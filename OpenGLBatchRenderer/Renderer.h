#pragma once

#define IMMEDIATE_MODE_RENDERING

#include "PointLight.h"
#include "DirectionalLight.h"
#include "Camera.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "VertexArray.h"
#include "RenderPass.h"
#include "Memory.h"
#include "Model.h"
#include "UniformBufferObject.h"
#include "SSBO.h"

#include <array>
#include <span>
#include <vector>



struct Renderable {
	Ref<Model> Model;
	glm::mat4 Transform;
};

struct Scene {
	std::vector<Renderable> BatchedRenderables;

	void clear() {
		BatchedRenderables.clear();
	}
};

#define size_assert(type, size) static_assert(sizeof(type) == (size), "Size of " #type " is not " #size)

class Renderer {
public:
	explicit Renderer();

	void begin_scene(const Camera& camera, const std::vector<PointLight>& lights, const DirectionalLight& dir_light, const Ref<Shader>& shader);
	void end_scene();

	void submit(const Ref<Model>& model, const glm::mat4& transform);
	void flush();

	// Immediate functions
	void clear() const;
	void draw_lines(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;
	void draw_triangles(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const;

	static inline constexpr size_t MaxVertices = 20000;
	static inline constexpr size_t MaxIndices = 20000;
	static inline constexpr size_t MaxTextures = 32;
	static inline constexpr size_t MaxMaterials = MaxTextures / 2; // 2 textures per material
	static inline constexpr size_t MaxTransforms = 1000;
	static inline constexpr size_t MaxPointLights = 16;

private:
	struct Vertex {
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoords;
		float MaterialIndex; // float because GPUs like floats
		float ModelIndex;
	};
	struct LightData {
		struct PointLight {
			alignas(16) glm::vec3 Position;	 // Offset: 0x00
			alignas(16) glm::vec3 Ambient;	 // Offset: 0x10
			alignas(16) glm::vec3 Diffuse;	 // Offset: 0x20
			alignas(16) glm::vec3 Specular;	 // Offset: 0x30
			float Power;					 // Offset: 0x3C
			float Constant;					 // Offset: 0x40
			float Linear;					 // Offset: 0x44
			float Quadratic;				 // Offset: 0x48
		};
		struct DirectionalLight {
			alignas(16) glm::vec3 Direction;
			alignas(16) glm::vec3 Ambient;
			alignas(16) glm::vec3 Diffuse;
			alignas(16) glm::vec3 Specular;
			float Power;
		};
		size_assert(PointLight, 80);
		size_assert(DirectionalLight, 64);

		PointLight PointLights[MaxPointLights];
		alignas(16) DirectionalLight DirLight;
	};
	size_assert(LightData, 1344);
	struct Material {
		int DiffuseMapIndex;
		int SpecularMapIndex;
		float Shininess;
	};
	size_assert(Material, 12);

	const Camera* m_camera{ nullptr };
	Ref<Shader> m_shader;

	Scope<UniformBufferObject> m_light_ubo;
	Scope<UniformBufferObject> m_camera_matrices_ubo;

	Scope<SSBO> m_transform_ssbo;
	Scope<glm::mat4[]> m_transforms_data;
	size_t m_transforms_index{ 0 };

	Scope<VertexBuffer> m_vertex_buffer;
	Scope<Vertex[]> m_vertex_buffer_data;
	Vertex* m_vertex_buffer_ptr{ nullptr };
	size_t m_vertex_buffer_index{ 0 };

	Scope<IndexBuffer> m_index_buffer;
	Scope<uint32_t[]> m_index_buffer_data;
	size_t m_index_buffer_index{ 0 };

	Scope<Material[]> m_materials_data;
	std::array<Ref<::Material>, MaxMaterials> m_used_materials;
	size_t m_materials_index{ 0 };
	size_t m_textures_index{ 0 };

	std::array<glm::mat4, 3> m_camera_matrices{};
	LightData m_light_data{};

	Scope<VertexArray> m_vertex_array;

	Scene m_scene{};
};

