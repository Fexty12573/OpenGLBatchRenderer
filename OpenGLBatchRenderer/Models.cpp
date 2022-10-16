#include "Models.h"

#include <fstream>
#include <sstream>
#include <string>
#include <ranges>
#include <unordered_map>

#include <fmt/format.h>
#include <scn/scn.h>

#include <zlib.h>

struct Indices {
	int vertex;
	int uv;
	int normal;

	uint32_t hash() const {
		const auto vcrc = crc32(-1, reinterpret_cast<const Bytef*>(&vertex), sizeof vertex);
		const auto ucrc = crc32(-1, reinterpret_cast<const Bytef*>(&uv), sizeof uv);
		const auto ncrc = crc32(-1, reinterpret_cast<const Bytef*>(&normal), normal);
		return vcrc ^ ucrc ^ ncrc;
	}
};

gl::Model gl::load_obj(const std::filesystem::path& path) {
	Model model{};

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

	std::string line;
	std::ifstream file(path);

	int i = 1;
	while (std::getline(file, line)) {
		if (line.starts_with("v ")) {
			float x, y, z;

			if (scn::scan(line, "v {} {} {}", x, y, z)) {
				vertices.emplace_back(x, y, z);
			} else {
				fmt::print("Failed to load OBJ. Invalid vertex format at line {}\n", i);
				break;
			}
		} else if (line.starts_with("vt ")) {
			float x, y;

			if (scn::scan(line, "vt {} {}", x, y)) {
				uvs.emplace_back(x, y);
			} else {
				fmt::print("Failed to load OBJ. Invalid UV format at line {}\n", i);
				break;
			}
		} else if (line.starts_with("vn ")) {
			float x, y, z;

			if (scn::scan(line, "vn {} {} {}", x, y, z)) {
				normals.emplace_back(x, y, z);
			} else {
				fmt::print("Failed to load OBJ. Invalid normal format at line {}\n", i);
				break;
			}
		} else if (line.starts_with("f ")) {
			int verts[3]{}, _uvs[3]{}, norms[3]{};

			if (scn::scan(line, "f {}/{}/{} {}/{}/{} {}/{}/{}", 
				verts[0], _uvs[0], norms[0], 
				verts[1], _uvs[1], norms[1], 
				verts[2], _uvs[2], norms[2])) {
				for (int j = 0; j < 3; j++) {
					model.vertices.emplace_back(vertices[verts[j] - 1]);
					model.uvs.emplace_back(uvs[_uvs[j] - 1]);
					model.normals.emplace_back(normals[norms[j] - 1]);
				}
			} else {
				fmt::print("Failed to load OBJ. Invalid face format at line {}\n", i);
				break;
			}
		}

		++i;
	}

	return model;
}

gl::Model2 gl::load_obj2(const std::filesystem::path& path) {
	Model2 model{};

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;

	//std::unordered_map<uint32_t, uint32_t> index_cache;

	std::string line;
	std::ifstream file(path);

	int i = 1;
	while (std::getline(file, line)) {
		if (line.starts_with("v ")) {
			float x, y, z;

			if (scn::scan(line, "v {} {} {}", x, y, z)) {
				vertices.emplace_back(x, y, z);
			} else {
				fmt::print("Failed to load OBJ. Invalid vertex format at line {}\n", i);
				break;
			}
		} else if (line.starts_with("vt ")) {
			float x, y;

			if (scn::scan(line, "vt {} {}", x, y)) {
				uvs.emplace_back(x, y);
			} else {
				fmt::print("Failed to load OBJ. Invalid UV format at line {}\n", i);
				break;
			}
		} else if (line.starts_with("vn ")) {
			float x, y, z;

			if (scn::scan(line, "vn {} {} {}", x, y, z)) {
				normals.emplace_back(x, y, z);
			} else {
				fmt::print("Failed to load OBJ. Invalid normal format at line {}\n", i);
				break;
			}
		} else if (line.starts_with("f ")) {
			Indices indices_list[3]{};

			if (scn::scan(line, "f {}/{}/{} {}/{}/{} {}/{}/{}",
				indices_list[0].vertex, indices_list[0].uv, indices_list[0].normal,
				indices_list[1].vertex, indices_list[1].uv, indices_list[1].normal,
				indices_list[2].vertex, indices_list[2].uv, indices_list[2].normal)) {
				for (auto& indices : indices_list) {
					/*const auto hash = indices.hash();

					if (index_cache.contains(hash)) {
						model.indices.push_back(index_cache[hash]);
						continue;
					}*/

					const auto index = static_cast<uint32_t>(model.vertices.size()) / 8U;

					const auto& v = vertices[indices.vertex - 1];
					const auto& uv = uvs[indices.uv - 1];
					const auto& n = normals[indices.normal - 1];

					model.indices.push_back(index);
					model.vertices.push_back(v.x);
					model.vertices.push_back(v.y);
					model.vertices.push_back(v.z);
					model.vertices.push_back(uv.x);
					model.vertices.push_back(uv.y);
					model.vertices.push_back(n.x);
					model.vertices.push_back(n.y);
					model.vertices.push_back(n.z);
				}
			} else {
				fmt::print("Failed to load OBJ. Invalid face format at line {}\n", i);
				break;
			}
		}

		++i;
	}

	return model;
}
