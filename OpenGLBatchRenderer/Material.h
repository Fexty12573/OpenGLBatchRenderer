#pragma once

#include <memory>
#include <span>
#include <vector>

#include <glm/glm.hpp>

#include "Memory.h"
#include "Texture.h"

class Material {
public:
	Material(std::span<Ref<Texture>> textures, float shininess);
	Material(const Material&) = default;
	Material(Material&&) = default;
	~Material() = default;

	Material& operator=(const Material&) = default;
	Material& operator=(Material&&) = default;

	Ref<Texture> get_texture(TextureType type) const;

public:
	std::vector<Ref<Texture>> Textures;

	float Shininess; // specular exponent

	// Not used yet
	/*float Transparency;
	float OpticalDensity;*/
};

