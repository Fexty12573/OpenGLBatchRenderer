#include "Mesh.h"

#include "Debug.h"
#include "VertexBufferLayout.h"

Mesh::Mesh(std::span<Vertex> vertices, std::span<unsigned int> indices, Ref<Material> material)
	: m_vertices(vertices.begin(), vertices.end())
	, m_indices(indices.begin(), indices.end())
	, m_material(std::move(material)) {

#ifdef IMMEDIATE_MODE_RENDERING
	m_vb = make_scope<VertexBuffer>(m_vertices.data(), static_cast<uint32_t>(vertices.size_bytes()));
	m_ib = make_scope<IndexBuffer>(m_indices.data(), static_cast<uint32_t>(m_indices.size()));
	m_va = make_scope<VertexArray>();

	VertexBufferLayout layout{};

	layout.push<float>(3); // Positions
	layout.push<float>(2); // UVs
	layout.push<float>(3); // Normals

	m_va->add_buffer(*m_vb, layout);
#endif
}

Mesh::~Mesh() = default;

#ifdef IMMEDIATE_MODE_RENDERING
void Mesh::draw(Shader& shader) const {
	shader.bind();
	int diffuse_num = 0, specular_num = 0;

	shader.set_uniform_1f("u_materials[0].shininess", m_material->Shininess);

	for (int i = 0; i < m_material->Textures.size(); i++) {
		m_material->Textures[i]->bind(i);

		switch (m_material->Textures[i]->get_type()) {
		case TextureType::None: 
			break;
		case TextureType::Diffuse:
			shader.set_uniform_1i("u_materials[" + std::to_string(diffuse_num) + "].diffuse", i);
			diffuse_num++;
			break;
		case TextureType::Specular:
			shader.set_uniform_1i("u_materials[" + std::to_string(specular_num) + "].specular", i);
			specular_num++;
			break;
		}
	}


	m_va->bind();
	m_ib->bind();
	gl_call(glDrawElements(GL_TRIANGLES, m_ib->count(), GL_UNSIGNED_INT, nullptr));
	m_va->unbind();
}
#endif
