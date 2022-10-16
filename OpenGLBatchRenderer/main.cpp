#include <fmt/format.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <map>

#include "Camera.h"
#include "Cubemap.h"
#include "Debug.h"
#include "IndexBuffer.h"
#include "Models.h"
#include "Renderer.h"
#include "Shader.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "Texture.h"
#include "Model.h"
#include "PointLight.h"

static bool g_free_mouse = false;
static bool g_lock_camera = false;
static std::unique_ptr<Camera> g_camera;

static constexpr float g_skybox_vertices[] = {
	// positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f, -1.0f,
	 1.0f,  1.0f,  1.0f,
	 1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f, -1.0f,
	 1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	 1.0f, -1.0f,  1.0f
};

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void resize_callback(GLFWwindow*, int width, int height);

int main(int argc, char** argv) {
	glewExperimental = GL_TRUE;
	if (!glfwInit()) {
		fmt::print("Failed to initialize GLFW\n");
		return -1;
	}

	constexpr int width = 1920;
	constexpr int height = 1080;
	constexpr float fov = 45.0f;
	
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	constexpr auto glsl_version = "#version 330 core";

	GLFWwindow* window = glfwCreateWindow(width, height, "OpenGL Test", nullptr, nullptr);
	if (!window) {
		fmt::print("Failed to create GLFW Window\n");
		return -1;
	}

	{
		glfwMakeContextCurrent(window);

		if (const auto result = glewInit(); result != GLEW_OK) {
			fmt::print("Failed to initialize GLEW: {}\n", reinterpret_cast<const char*>(glewGetErrorString(result)));
			return -1;
		}

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		auto& io = ImGui::GetIO(); (void)io;

		ImGui::StyleColorsDark();

		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init(glsl_version);

		glfwSetKeyCallback(window, key_callback);
		glfwSetWindowSizeCallback(window, resize_callback);

		glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

		gl_call(glEnable(GL_LINE_SMOOTH));
		gl_call(glEnable(GL_CULL_FACE));
		gl_call(glEnable(GL_DEPTH_TEST));
		gl_call(glEnable(GL_BLEND));
		gl_call(glDepthFunc(GL_LEQUAL));
		gl_call(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		float width_range[2] = { 0, 0 };
		glGetFloatv(GL_SMOOTH_LINE_WIDTH_RANGE, width_range);
		fmt::print("Line Thickness Limits: {{ {}, {} }}\n", width_range[0], width_range[1]);

		const auto cube = gl::load_obj2("./res/models/cube_16x16.obj");
		VertexBuffer cube_vb(cube.vertices.data(), cube.vertices.size() * sizeof(float));
		IndexBuffer cube_ib(cube.indices.data(), cube.indices.size());
		VertexArray cube_va;

		VertexBufferLayout pos_only_layout{};
		pos_only_layout.push<float>(3);

		VertexBufferLayout default_layout{};
		default_layout.push<float>(3); // Vertices
		default_layout.push<float>(2); // UVs
		default_layout.push<float>(3); // Normals

		cube_va.add_buffer(cube_vb, default_layout);

		VertexBuffer skybox_vb(g_skybox_vertices, sizeof g_skybox_vertices);
		VertexArray skybox_va;
		skybox_va.add_buffer(skybox_vb, pos_only_layout);

		g_camera = std::make_unique<Camera>(fov, 0.1f, 100.0f);

		Shader default_shader("./res/shaders/default_shading.vert", "./res/shaders/default_shading.frag");
		Shader light_shader("./res/shaders/point_light.vert", "./res/shaders/point_light.frag");
		Shader skybox_shader("./res/shaders/skybox.vert", "./res/shaders/skybox.frag");
		Texture box_texture("./res/textures/container2.png", { .type = TextureType::Diffuse });
		Texture box_specular_texture("./res/textures/container2_specular.png", { .type = TextureType::Specular });

		auto batch_shader = make_ref<Shader>("./res/shaders/default_shading_instanced.vert", "./res/shaders/default_shading_instanced.frag");

		auto backpack = make_ref<Model>("./res/models/backpack/backpack.obj");
		auto floor = make_ref<Model>("./res/models/floor/floor.obj");
		auto fish = make_ref<Model>("./res/models/fish/fish.obj");

		std::vector<std::pair<const char*, std::shared_ptr<Cubemap>>> cubemaps = {
			{"./res/textures/skybox", nullptr},
			{"./res/textures/sky1", nullptr},
			{"./res/textures/sky2", nullptr},
			{"./res/textures/space", nullptr},
			{"./res/textures/forest", nullptr},
			{"./res/textures/yokohama3", nullptr},
			{"./res/textures/river", nullptr}
		};

		for (auto& [path, cubemap] : cubemaps) {
			cubemap = std::make_shared<Cubemap>(path);
		}

		Renderer renderer{};

		std::vector positions = {
			2.0f * glm::vec3(-3.3, 3.0, 2.0),
			2.0f * glm::vec3(2.0, 5.0, -5.0),
			2.0f * glm::vec3(-1.5, -2.2, -2.5),
			2.0f * glm::vec3(-3.8, -2.0, 4.3),
			2.0f * glm::vec3(2.4, -0.4, -3.5),
			2.0f * glm::vec3(-1.7, 3.0, -7.5),
			2.0f * glm::vec3(1.3, -2.0, -2.5),
			2.0f * glm::vec3(1.5, 2.0, -2.5),
			2.0f * glm::vec3(1.5, 0.2, -1.5),
			2.0f * glm::vec3(-1.3, 1.0, -1.5)
		};
		std::vector<glm::vec3> fish_positions;

		VertexBuffer positions_buffer(positions.data(), positions.size() * sizeof(glm::vec3), GL_DYNAMIC_DRAW);
		cube_va.add_instanced_buffer(positions_buffer, pos_only_layout, 1);

		DirectionalLight dir_light(glm::vec3(-0.2f, -1.0f, -0.3f));
		std::vector lights = {
			PointLight(glm::vec3(4.7, 4.2, 2.0)),
			PointLight(glm::vec3(2.3, -5.3, -8.0)),
			PointLight(glm::vec3(-7.0, 2.0, -12.0)),
			PointLight(glm::vec3(0.0, 1.0, -7.0))
		};

		default_shader.set_uniform("test", 0.0);

		int current_cubemap = 0;
		double current_frame = glfwGetTime();
		double last_frame = current_frame;
		glfwSetCursorPos(window, width / 2.0, height / 2.0);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		while (!glfwWindowShouldClose(window)) {
			current_frame = glfwGetTime();
			const double dt = current_frame - last_frame;
			last_frame = current_frame;

			renderer.clear();

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			ImGui::SetNextWindowSize({ 400, 180 }, ImGuiCond_Once);
			ImGui::Begin("Debug");

			ImGui::Text("FPS: %.1f", 1.0 / dt);
			ImGui::Text("Camera Position: %.1f, %.1f, %.1f", g_camera->get_position().x, g_camera->get_position().y, g_camera->get_position().z);
			ImGui::Text("Camera Direction: %.1f, %.1f, %.1f", g_camera->get_direction().x, g_camera->get_direction().y, g_camera->get_direction().z);

			if (ImGui::Button("Reload Default Shader")) {
				default_shader.reload();
			}

			if (ImGui::BeginCombo("Skybox", cubemaps[current_cubemap].first)) {
				for (int i = 0; i < cubemaps.size(); ++i) {
					if (ImGui::Selectable(cubemaps[i].first)) {
						current_cubemap = i;
					}
				}
				ImGui::EndCombo();
			}

			for (auto i = 0u; i < lights.size(); ++i) {
				auto& light = lights[i];

				if (ImGui::TreeNode(fmt::format("Light {}", i).c_str())) {
					light.draw_imgui_options();
					ImGui::TreePop();
				}
			}

			if (ImGui::TreeNode("Cube")) {
				ImGui::DragFloat3("Position", glm::value_ptr(positions[0]), 0.1f);
				ImGui::TreePop();
			}

			ImGui::End();
			ImGui::Render();

			bool accept_input = true;
			if (g_lock_camera) {
				accept_input = false;
				g_camera->look_at({ 0, 5, 0 });
			}

			if (!g_free_mouse) {
				g_camera->update(static_cast<float>(dt), accept_input);
			}

			const auto& view = g_camera->get_view_matrix();
			const auto& projection = g_camera->get_projection_matrix();

			light_shader.bind();
			light_shader.set_uniform("u_view", view);
			light_shader.set_uniform("u_projection", projection);

			for (const auto& light : lights) {
				light.upload_to_light_shader(light_shader);
				renderer.draw_triangles(cube_va, cube_ib, light_shader);
			}

			default_shader.bind();
			box_texture.bind(0);
			box_specular_texture.bind(1);
			cubemaps[current_cubemap].second->bind(2);

			default_shader.set_uniform("u_skybox", 2);
			default_shader.set_uniform("u_view_pos", g_camera->get_position());

			for (auto i = 0u; i < lights.size(); ++i) {
				lights[i].upload_to_shader(default_shader, fmt::format("u_lights[{}]", i));
			}

			default_shader.set_uniform("u_dir_light.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
			default_shader.set_uniform("u_dir_light.ambient", glm::vec3(0.2f));
			default_shader.set_uniform("u_dir_light.diffuse", glm::vec3(0.5f));
			default_shader.set_uniform("u_dir_light.specular", glm::vec3(1.0f));
			default_shader.set_uniform("u_dir_light.power", 1.31f);

			default_shader.set_uniform("u_materials[0].diffuse", 0);
			default_shader.set_uniform("u_materials[0].specular", 1);
			default_shader.set_uniform("u_materials[0].shininess", 33.0f);

			default_shader.set_uniform("u_mvp", projection * view * glm::mat4(1.0f));
			default_shader.set_uniform("u_model", glm::mat4(1.0f));
			default_shader.set_uniform("u_normal_matrix", glm::transpose(glm::inverse(glm::mat3(1.0f))));
			backpack->draw(default_shader);

			auto model = glm::mat4(1.0f);
			model = glm::translate(model, { 0.0f, -5.0f, 0.0f });
			model = glm::scale(model, { 10.0f, 1.0f, 10.0f });
			default_shader.set_uniform("u_mvp", projection * view * model);
			default_shader.set_uniform("u_model", model);
			default_shader.set_uniform("u_normal_matrix", glm::transpose(glm::inverse(glm::mat3(model))));

			floor->draw(default_shader);

			renderer.begin_scene(*g_camera, lights, dir_light, batch_shader);

			for (auto i = 0u; i < positions.size(); ++i) {
				model = glm::mat4(1.0f);
				model = glm::translate(model, positions[i]);
				model = glm::rotate(model, static_cast<float>(current_frame) * static_cast<float>(i) * glm::radians(1.0f),
					glm::vec3(1.0f, 0.3f, 0.5f));
				renderer.submit(backpack, model);
			}

			renderer.end_scene();

			// Render the skybox last
			glDepthMask(GL_FALSE);
			glDisable(GL_CULL_FACE);
			skybox_shader.bind();
			cubemaps[current_cubemap].second->bind();

			skybox_shader.set_uniform("u_view", glm::mat4(glm::mat3(view)));
			skybox_shader.set_uniform("u_projection", projection);
			skybox_shader.set_uniform("u_skybox", 0);

			renderer.draw_triangles(cube_va, cube_ib, skybox_shader);
			glDepthMask(GL_TRUE);
			glEnable(GL_CULL_FACE);

			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			glfwSwapBuffers(window);
			glfwPollEvents();
		}

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, true);
			break;
		case GLFW_KEY_TAB:
			g_free_mouse = !g_free_mouse;
			fmt::print("Free mouse: {}\n", g_free_mouse);
			break;
		case GLFW_KEY_L:
			g_lock_camera = !g_lock_camera;
			fmt::print("Lock camera: {}\n", g_lock_camera);
			break;
		default:
			break;
		}
	}

	if (g_free_mouse) {
		ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
	}
}

void resize_callback(GLFWwindow*, int width, int height) {
	g_camera->on_resize(width, height);
}
