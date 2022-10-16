#pragma once

#include <memory>
#include <vector>
#include <span>

#include <glm/glm.hpp>

#include "Material.h"
#include "Memory.h"
#include "Texture.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"

#pragma pack(push, 1)
struct Vertex {
	glm::vec3 position;
	glm::vec2 tex_coords;
	glm::vec3 normal;
};
#pragma pack(pop)

class Mesh {
public:
	Mesh(std::span<Vertex> vertices, std::span<unsigned int> indices, Ref<Material> material);
	Mesh(const Mesh& other) = delete;
	Mesh(Mesh&& other) noexcept = default;
	~Mesh();

#ifdef IMMEDIATE_MODE_RENDERING
	/**
	 * \brief Draws the mesh using the provided shader.
	 *
	 * \summary The provided shader requires a struct called Material with at least the following members:
	 * - sampler2D diffuse;
	 * - sampler2D specular;
	 * - float shininess;
	 * The textures will be uploaded to the shader using the following names:
	 * - u_materials[N].diffuse
	 * - u_materials[N].specular
	 * - u_materials[0].shininess
	 *
	 * where N is the number of the texture.
	 *
	 * \param shader The shader to use.
	 */
	void draw(Shader& shader) const;

	[[nodiscard]] const auto& get_vertex_array() const { return m_va; }
	[[nodiscard]] const auto& get_vertex_buffer() const { return m_vb; }
	[[nodiscard]] const auto& get_index_buffer() const { return m_ib; }
#endif

	[[nodiscard]] const auto& get_vertices() const { return m_vertices; }
	[[nodiscard]] const auto& get_indices() const { return m_indices; }
	[[nodiscard]] const auto& get_material() const { return m_material; }

private:
	std::vector<Vertex> m_vertices;
	std::vector<uint32_t> m_indices;
	Ref<Material> m_material;

#ifdef IMMEDIATE_MODE_RENDERING
	Scope<VertexArray> m_va;
	Scope<VertexBuffer> m_vb;
	Scope<IndexBuffer> m_ib;
#endif
};

