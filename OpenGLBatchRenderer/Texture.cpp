#include "Texture.h"

#include "Debug.h"

#include <stb_image.h>
#include <GL/glew.h>

Texture::Texture(std::filesystem::path path, const TextureParameters& params)
	: m_renderer_id(0), m_filepath(std::move(path)), m_local_buffer(nullptr), m_width(0), m_height(0), m_bpp(0) {
	stbi_set_flip_vertically_on_load(true);

	m_local_buffer = stbi_load(m_filepath.string().c_str(), &m_width, &m_height, &m_bpp, 4);
	if (!m_local_buffer) {
		fmt::print("Failed to load texture ({}): {}\n", m_filepath.string(), stbi_failure_reason());
	}

	gl_call(glGenTextures(1, &m_renderer_id));
	gl_call(glBindTexture(GL_TEXTURE_2D, m_renderer_id));

	gl_call(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, params.min_filter));
	gl_call(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, params.mag_filter));
	gl_call(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, params.wrap_s));
	gl_call(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, params.wrap_t));

	gl_call(glTexImage2D(GL_TEXTURE_2D, 0, params.internal_format, m_width, m_height, 0, params.format, GL_UNSIGNED_BYTE, m_local_buffer));
	unbind();

	fmt::print("Loaded texture ({}): {}x{}x{} with ID {}\n", m_filepath.string(), m_width, m_height, m_bpp, m_renderer_id);

	m_format = params.format;
	m_internal_format = params.internal_format;
	m_type = params.type;

	if (m_local_buffer) {
		stbi_image_free(m_local_buffer);
		m_local_buffer = nullptr;
	}
}

Texture::Texture(int width, int height, const TextureParameters& params) : m_renderer_id(0) {
	gl_call(glGenTextures(1, &m_renderer_id));
	gl_call(glBindTexture(GL_TEXTURE_2D, m_renderer_id));

	gl_call(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, params.wrap_s));
	gl_call(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, params.wrap_t));
	gl_call(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, params.min_filter));
	gl_call(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, params.mag_filter));

	gl_call(glTexImage2D(GL_TEXTURE_2D, 0, params.internal_format, width, height,
		0, params.format, GL_UNSIGNED_BYTE, nullptr));
	gl_call(glGenerateMipmap(GL_TEXTURE_2D));

	m_width = width;
	m_height = height;
	m_bpp = 4; // TODO: Fix this
	m_local_buffer = nullptr;
	m_filepath = "";
	m_format = params.format;
	m_internal_format = params.internal_format;
	m_type = params.type;
}

Texture::~Texture() {
	gl_call(glDeleteTextures(1, &m_renderer_id));
}

void Texture::bind(uint32_t slot) const {
	gl_call(glActiveTexture(GL_TEXTURE0 + slot));
	gl_call(glBindTexture(GL_TEXTURE_2D, m_renderer_id));
}

void Texture::unbind() const {
	gl_call(glBindTexture(GL_TEXTURE_2D, 0));
}

void Texture::resize(int width, int height) {
	bind();
	gl_call(glTexImage2D(GL_TEXTURE_2D, 0, m_internal_format, width, height, 0, m_format, GL_UNSIGNED_BYTE, nullptr));
	unbind();

	m_width = width;
	m_height = height;
}

void Texture::activate(uint32_t slot) {
	gl_call(glActiveTexture(GL_TEXTURE0 + slot));
}

