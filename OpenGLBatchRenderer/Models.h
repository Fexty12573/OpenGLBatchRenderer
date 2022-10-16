#pragma once

#include <filesystem>
#include <vector>

#include <glm/glm.hpp>



namespace gl {

struct Model {
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
};
struct Model2 {
	std::vector<float> vertices;
	std::vector<uint32_t> indices;

	// Layout of 'vertices':
	// x, y, z, u, v, nx, ny, nz [0]
	// x, y, z, u, v, nx, ny, nz [1]
	// x, y, z, u, v, nx, ny, nz [2]
	// ...
};

Model load_obj(const std::filesystem::path& path);
Model2 load_obj2(const std::filesystem::path& path);

}
