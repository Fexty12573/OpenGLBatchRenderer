#include "Renderer.h"

#include "Debug.h"
#include "VertexBufferLayout.h"
	
#include <GL/glew.h>
#include <GLFW/glfw3.h>

Renderer::Renderer() {
	m_light_ubo = make_scope<UniformBufferObject>(sizeof LightData);

	m_camera_matrices_ubo = make_scope<UniformBufferObject>(sizeof(glm::mat4) * 3);

	m_transform_ssbo = make_scope<SSBO>(MaxTransforms * 2 * sizeof(glm::mat4), GL_DYNAMIC_DRAW);
	m_transforms_data = make_scope<glm::mat4[]>(MaxTransforms * 2); // 2x because we need to store the inverse of the model matrix

	m_vertex_buffer = make_scope<VertexBuffer>(nullptr, MaxVertices * sizeof(Vertex), GL_DYNAMIC_DRAW);
	m_vertex_buffer_data = make_scope<Vertex[]>(MaxVertices);

	m_index_buffer = make_scope<IndexBuffer>(nullptr, MaxVertices, GL_DYNAMIC_DRAW);
	m_index_buffer_data = make_scope<uint32_t[]>(MaxVertices);

	m_materials_data = make_scope<Material[]>(MaxMaterials);

	std::memset(m_transforms_data.get(), 0, MaxTransforms * sizeof(glm::mat4));
	std::memset(m_vertex_buffer_data.get(), 0, MaxVertices * sizeof(Vertex));
	std::memset(m_index_buffer_data.get(), 0, MaxVertices * sizeof(uint32_t));

	m_vertex_array = make_scope<VertexArray>();

	VertexBufferLayout layout;
	layout.push<float>(3); // Position
	layout.push<float>(3); // Normal
	layout.push<float>(2); // TexCoords
	layout.push<float>(1); // MaterialIndex
	layout.push<float>(1); // TransformIndex

	m_vertex_array->add_buffer(*m_vertex_buffer, layout);
}

void Renderer::begin_scene(const Camera& camera, const std::vector<PointLight>& lights, const DirectionalLight& dir_light, const Ref<Shader>& shader) {
	m_scene.clear();

	m_camera = &camera;
	m_shader = shader;

	m_vertex_buffer_ptr = m_vertex_buffer_data.get();
	m_vertex_buffer_index = 0;
	m_index_buffer_index = 0;
	m_transforms_index = 0;
	m_materials_index = 0;

	shader->bind();
	shader->set_uniform("u_ViewPos", camera.get_position());
	shader->set_uniform("u_Time", static_cast<float>(glfwGetTime()));

	// Copy the light data into structures that can be passed to the shader with std140 layout
	for (auto i = 0u; i < std::min(MaxPointLights, lights.size()); ++i) {
		m_light_data.PointLights[i].Position = lights[i].m_position;
		m_light_data.PointLights[i].Ambient = lights[i].m_ambient;
		m_light_data.PointLights[i].Diffuse = lights[i].m_diffuse;
		m_light_data.PointLights[i].Specular = lights[i].m_specular;
		m_light_data.PointLights[i].Power = lights[i].m_power;
		m_light_data.PointLights[i].Constant = lights[i].m_constant;
		m_light_data.PointLights[i].Linear = lights[i].m_linear;
		m_light_data.PointLights[i].Quadratic = lights[i].m_quadratic;
	}
	
	m_light_data.DirLight.Direction = dir_light.m_direction;
	m_light_data.DirLight.Ambient = dir_light.m_ambient;
	m_light_data.DirLight.Diffuse = dir_light.m_diffuse;
	m_light_data.DirLight.Specular = dir_light.m_specular;
	m_light_data.DirLight.Power = dir_light.m_power;

	m_light_ubo->update(&m_light_data, sizeof m_light_data);

	m_camera_matrices[0] = camera.get_view_matrix();
	m_camera_matrices[1] = camera.get_projection_matrix();
	m_camera_matrices[2] = m_camera_matrices[1] * m_camera_matrices[0];
	m_camera_matrices_ubo->update(m_camera_matrices.data());

	shader->bind_ubo("Camera", 0);
	m_camera_matrices_ubo->bind_to_slot(0);
}

void Renderer::end_scene() {
	flush();
}

void Renderer::submit(const Ref<Model>& model, const glm::mat4& transform) {
	const glm::mat4 normal_matrix = glm::transpose(glm::inverse(transform));

	for (auto& mesh : model->get_meshes()) {
		m_transforms_data[m_transforms_index] = transform;
		m_transforms_data[m_transforms_index + 1] = normal_matrix;

		if (m_vertex_buffer_index + mesh.get_vertices().size() > MaxVertices || 
			m_index_buffer_index + mesh.get_indices().size() > MaxIndices) {
			flush();
		}

		const auto& material = mesh.get_material();

		// Check if the material is already in the buffer
		auto material_index = 0u;
		for (; material_index < m_materials_index; ++material_index) {
			if (m_used_materials[material_index] == material) {
				break;
			}
		}

		if (material_index == m_materials_index) {
			if (m_materials_index == MaxMaterials) {
				flush();
				material_index = 0;
			}

			// If the material is not in the buffer, add it
			m_used_materials[material_index] = material;
			m_materials_data[material_index].DiffuseMapIndex = static_cast<int>(m_textures_index);
			m_materials_data[material_index].SpecularMapIndex = static_cast<int>(m_textures_index + 1ULL);
			m_materials_data[material_index].Shininess = material->Shininess;

			m_materials_index += 1;
			m_textures_index += 2;
		}

		for (const uint32_t index : mesh.get_indices()) {
			m_index_buffer_data[m_index_buffer_index++] = index + static_cast<uint32_t>(m_vertex_buffer_index);
		}

		for (const auto& vertex : mesh.get_vertices()) {
			m_vertex_buffer_ptr->Position = vertex.position;
			m_vertex_buffer_ptr->Normal = vertex.normal;
			m_vertex_buffer_ptr->TexCoords = vertex.tex_coords;
			m_vertex_buffer_ptr->ModelIndex = static_cast<float>(m_transforms_index);
			m_vertex_buffer_ptr->MaterialIndex = static_cast<float>(material_index);
			m_vertex_buffer_ptr++;
			m_vertex_buffer_index++;
		}
	}

	m_transforms_index += 2; // Transform and normal matrix
}

void Renderer::clear() const {
	gl_call(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void Renderer::draw_lines(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const {
	shader.bind();
	va.bind();
	ib.bind();

	gl_call(glDrawElements(GL_LINES, ib.count(), GL_UNSIGNED_INT, nullptr));
}

void Renderer::draw_triangles(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const {
	shader.bind();
	va.bind();
	ib.bind();

	gl_call(glDrawElements(GL_TRIANGLES, ib.count(), GL_UNSIGNED_INT, nullptr));
}

void Renderer::flush() {
	m_shader->bind();

	// Bind all materials
	for (auto i = 0u; i < m_materials_index; ++i) {
		m_used_materials[i]->get_texture(TextureType::Diffuse)->bind(m_materials_data[i].DiffuseMapIndex);
		m_used_materials[i]->get_texture(TextureType::Specular)->bind(m_materials_data[i].SpecularMapIndex);

		std::string mat = fmt::format("Materials[{}]", i);
		m_shader->set_uniform(mat + ".diffuse", m_materials_data[i].DiffuseMapIndex);
		m_shader->set_uniform(mat + ".specular", m_materials_data[i].SpecularMapIndex);
		m_shader->set_uniform(mat + ".shininess", m_materials_data[i].Shininess);
	}

	m_index_buffer->update(m_index_buffer_data.get(), static_cast<uint32_t>(m_index_buffer_index));
	m_vertex_buffer->update(m_vertex_buffer_data.get(), static_cast<uint32_t>(m_vertex_buffer_index * sizeof(Vertex)));
	m_transform_ssbo->update(m_transforms_data.get(), m_transforms_index * sizeof(glm::mat4));
	m_transform_ssbo->bind_to_slot(0);

	m_vertex_array->bind();
	m_index_buffer->bind();
	gl_call(glDrawElements(GL_TRIANGLES, static_cast<int>(m_index_buffer_index), GL_UNSIGNED_INT, nullptr));

	m_vertex_buffer_index = 0;
	m_index_buffer_index = 0;
	m_materials_index = 0;
	m_textures_index = 0;
	m_transforms_index = 0;

	m_vertex_buffer_ptr = m_vertex_buffer_data.get();
}
