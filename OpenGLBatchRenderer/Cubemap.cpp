#include "Cubemap.h"

#include <array>

#include <gl/glew.h>
#include <stb_image.h>

#include "Debug.h"

Cubemap::Cubemap(const std::filesystem::path& path, const std::string& extension) : m_renderer_id(0) {
	gl_call(glGenTextures(1, &m_renderer_id));
	gl_call(glBindTexture(GL_TEXTURE_CUBE_MAP, m_renderer_id));

	std::string _extension = extension;
	if (extension == "auto") {
		_extension = std::filesystem::directory_iterator(path)->path().extension().string();
	}

	constexpr std::array filenames1 = {
		"right",
		"left",
		"top",
		"bottom",
		"front",
		"back"
	};
	constexpr std::array filenames2{
		"posx",
		"negx",
		"posy",
		"negy",
		"posz",
		"negz"
	};

	for (auto i = 0u; i < 6; i++) {
		const auto option1 = path / (filenames1[i] + _extension);
		const auto option2 = path / (filenames2[i] + _extension);

		if (std::filesystem::exists(option1)) {
			m_paths.push_back(option1);
		} else if (std::filesystem::exists(option2)) {
			m_paths.push_back(option2);
		} else {
			gl_error("Cubemap: Could not find file: {}", m_paths[i].string());
			throw std::runtime_error("Could not find cubemap texture: " + option1.string() + " or " + option2.string());
		}

		stbi_set_flip_vertically_on_load(false);
		int width, height, channels;
		uint8_t* data = stbi_load(m_paths[i].string().c_str(), &width, &height, &channels, 3);

		if (data) {
			gl_call(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
			stbi_image_free(data);
		} else {
			gl_error("Failed to load cubemap texture: {}", m_paths[i].string());
			stbi_image_free(data);
		}
	}

	gl_call(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	gl_call(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	gl_call(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	gl_call(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	gl_call(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
}

Cubemap::Cubemap(std::initializer_list<std::filesystem::path> paths) : m_renderer_id(0), m_paths(paths) {
	gl_call(glGenTextures(1, &m_renderer_id));
	gl_call(glBindTexture(GL_TEXTURE_CUBE_MAP, m_renderer_id));

	for (auto i = 0u; i < m_paths.size(); i++) {
		if (!std::filesystem::exists(m_paths[i])) {
			gl_error("Cubemap: Could not find file: {}", m_paths[i].string());
			return;
		}

		stbi_set_flip_vertically_on_load(false);
		int width, height, channels;
		uint8_t* data = stbi_load(m_paths[i].string().c_str(), &width, &height, &channels, 0);

		if (data) {
			gl_call(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data));
			stbi_image_free(data);
		} else {
			gl_error("Failed to load cubemap texture: {}", m_paths[i].string());
			stbi_image_free(data);
		}
	}
}

Cubemap::Cubemap(Cubemap&& cubemap) noexcept : m_renderer_id(cubemap.m_renderer_id), m_paths(std::move(cubemap.m_paths)) {
	cubemap.m_renderer_id = 0;
}

Cubemap::~Cubemap() {
	gl_call(glDeleteTextures(1, &m_renderer_id));
}

Cubemap& Cubemap::operator=(Cubemap&& cubemap) noexcept {
	if (this != &cubemap) {
		gl_call(glDeleteTextures(1, &m_renderer_id));

		m_renderer_id = cubemap.m_renderer_id;
		m_paths = std::move(cubemap.m_paths);

		cubemap.m_renderer_id = 0;
	}

	return *this;
}


void Cubemap::bind(int slot) const {
	gl_call(glActiveTexture(GL_TEXTURE0 + slot));
	gl_call(glBindTexture(GL_TEXTURE_CUBE_MAP, m_renderer_id));
}

void Cubemap::unbind() const {
	gl_call(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
}
