#include "Material.h"

Material::Material(std::span<Ref<Texture>> textures, float shininess)
	: Textures(textures.begin(), textures.end()), Shininess(shininess)/*, Transparency(0.0f), OpticalDensity(0.0f)*/ {
	if (Shininess == 0.0f) {
		Shininess = 33.0f;
	}
}

Ref<Texture> Material::get_texture(TextureType type) const {
	for (const auto& texture : Textures) {
		if (texture->get_type() == type) {
			return texture;
		}
	}

	return nullptr;
}
