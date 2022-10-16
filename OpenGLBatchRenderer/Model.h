#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <filesystem>

#include "Memory.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"

class Model {
public:
	explicit Model(const std::filesystem::path& path);
	~Model();

#ifdef IMMEDIATE_MODE_RENDERING
	void draw(Shader& shader) const;
#endif

	[[nodiscard]] const auto& get_meshes() const { return m_meshes; }

private:
	void load_model(const std::filesystem::path& path);
	void process_node(const aiNode* node, const aiScene* scene);
	Mesh process_mesh(const aiMesh* mesh, const aiScene* scene);
	std::vector<Ref<Texture>> load_material_textures(const aiMaterial* material, aiTextureType type, TextureType texture_type);

private:
	std::vector<Mesh> m_meshes;
	std::filesystem::path m_path;

	std::vector<Ref<Texture>> m_loaded_textures;
};
