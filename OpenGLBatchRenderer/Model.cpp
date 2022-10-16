#include "Model.h"

#include "Debug.h"

Model::Model(const std::filesystem::path& path) {
	m_path = path;
	load_model(path);
}

Model::~Model() = default;

#ifdef IMMEDIATE_MODE_RENDERING
void Model::draw(Shader& shader) const {
	shader.bind();
	for (const auto& mesh : m_meshes) {
		mesh.draw(shader);
	}
}
#endif

void Model::load_model(const std::filesystem::path& path) {
	Assimp::Importer importer{};
	const auto scene = importer.ReadFile(path.string(), aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		gl_error("Failed to load model: {}", importer.GetErrorString());
		return;
	}

	const auto dir = path.parent_path();

	process_node(scene->mRootNode, scene);
}

void Model::process_node(const aiNode* node, const aiScene* scene) {
	for (auto i = 0u; i < node->mNumMeshes; ++i) {
		const auto mesh = scene->mMeshes[node->mMeshes[i]];
		m_meshes.push_back(process_mesh(mesh, scene));
	}

	for (auto i = 0u; i < node->mNumChildren; ++i) {
		process_node(node->mChildren[i], scene);
	}
}

Mesh Model::process_mesh(const aiMesh* mesh, const aiScene* scene) {
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	std::vector<Ref<Texture>> textures;
	float shininess = 0.0f;

	for (auto i = 0u; i < mesh->mNumVertices; ++i) {
		const auto vp = mesh->mVertices + i;
		const auto vn = mesh->mNormals + i;
		glm::vec2 vt{};

		if (mesh->mTextureCoords[0]) {
			vt.x = mesh->mTextureCoords[0][i].x;
			vt.y = mesh->mTextureCoords[0][i].y;
		}

		vertices.push_back({ {vp->x, vp->y, vp->z}, vt, {vn->x, vn->y, vn->z} });
	}

	for (auto i = 0u; i < mesh->mNumFaces; ++i) {
		const auto face = mesh->mFaces + i;

		for (auto j = 0u; j < face->mNumIndices; ++j) {
			indices.push_back(face->mIndices[j]);
		}
	}


	if (mesh->mMaterialIndex >= 0) {
		const auto material = scene->mMaterials[mesh->mMaterialIndex];
		const auto diffuse_maps = load_material_textures(material, aiTextureType_DIFFUSE, TextureType::Diffuse);
		const auto specular_maps = load_material_textures(material, aiTextureType_SPECULAR, TextureType::Specular);
		material->Get(AI_MATKEY_SHININESS, shininess);

		textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());
		textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());
	}


	return { vertices, indices, make_ref<Material>(textures, shininess) };
}

std::vector<Ref<Texture>> Model::load_material_textures(const aiMaterial* material, aiTextureType type, TextureType texture_type) {
	std::vector<Ref<Texture>> textures;

	for (auto i = 0u; i < material->GetTextureCount(type); ++i) {
		aiString str;
		material->GetTexture(type, i, &str);

		const auto path = m_path.parent_path() / str.C_Str();

		const auto found = std::ranges::find_if(m_loaded_textures, [&path](const auto& texture) {
			return texture->get_path() == path;
		});

		if (found == m_loaded_textures.end()) {
			textures.push_back(make_ref<Texture>(path, TextureParameters{ .type = texture_type}));
			m_loaded_textures.push_back(textures.back());
		} else {
			textures.push_back(*found);
		}
	}

	return textures;
}
