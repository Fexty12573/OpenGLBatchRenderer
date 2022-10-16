#pragma once

#include <filesystem>
#include <GL/glew.h>

class Framebuffer;

enum class TextureType {
	None,
	Diffuse,
	Specular,
};

struct TextureParameters {
	GLint min_filter = GL_LINEAR;
	GLint mag_filter = GL_LINEAR;
	GLint wrap_s = GL_REPEAT;
	GLint wrap_t = GL_REPEAT;
	GLenum format = GL_RGBA;
	GLint internal_format = GL_RGBA;
	TextureType type = TextureType::None;
};

class Texture {
public:
	explicit Texture(std::filesystem::path path, const TextureParameters& params = {});
	explicit Texture(int width, int height, const TextureParameters& params = {});
	Texture(const Texture& t) = delete;
	Texture(Texture&&) noexcept = default; // TODO: Implement move constructor and move assignment operator
	~Texture();

	Texture& operator=(const Texture& t) = delete;
	Texture& operator=(Texture&&) noexcept = default;

	void bind(uint32_t slot = 0) const;
	void unbind() const;
	void resize(int width, int height);

	[[nodiscard]] int get_width() const { return m_width; }
	[[nodiscard]] int get_height() const { return m_height; }
	[[nodiscard]] int get_bpp() const { return m_bpp; }
	[[nodiscard]] TextureType get_type() const { return m_type; }
	[[nodiscard]] const std::filesystem::path& get_path() const { return m_filepath; }

	static void activate(uint32_t slot);

	friend class Framebuffer;

private:
	uint32_t m_renderer_id;
	std::filesystem::path m_filepath;
	uint8_t* m_local_buffer;
	int m_width, m_height, m_bpp;
	TextureType m_type;

	GLenum m_format;
	GLint m_internal_format;
};

