#pragma once

#include <gl/glew.h>

#include <vector>

#include "Texture.h"
#include "Renderbuffer.h"

class Framebuffer {
public:
	Framebuffer(int width, int height);
	Framebuffer(const Framebuffer&) = delete;
	Framebuffer(Framebuffer&&) = delete;
	~Framebuffer();

	Framebuffer& operator=(const Framebuffer&) = delete;
	Framebuffer& operator=(Framebuffer&&) = delete;

	void bind() const;
	void unbind() const;

	void attach_texture(int width = -1, int height = -1, int attachment = GL_COLOR_ATTACHMENT0, uint32_t format = GL_RGBA, int internal_format = GL_RGBA);
	void attach_renderbuffer(int width = -1, int height = -1, int attachment = GL_DEPTH_ATTACHMENT, int format = GL_DEPTH_COMPONENT);

	[[nodiscard]] const Texture& get_texture(int index) const;
	[[nodiscard]] const Renderbuffer& get_renderbuffer(int index) const;

	[[nodiscard]] bool is_complete() const;

	template<class T> const T& get_attachment(int index) const;
	void resize(int width, int height);

private:
	int m_width;
	int m_height;
	uint32_t m_renderer_id;

	std::vector<Texture> m_textures;
	std::vector<Renderbuffer> m_renderbuffers;
};

template<> const Texture& Framebuffer::get_attachment(int index) const {
	return get_texture(index);
}

template<> const Renderbuffer& Framebuffer::get_attachment(int index) const {
	return get_renderbuffer(index);
}
