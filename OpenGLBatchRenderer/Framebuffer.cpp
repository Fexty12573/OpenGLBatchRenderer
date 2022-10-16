#include "Framebuffer.h"

#include "Debug.h"

Framebuffer::Framebuffer(int width, int height) : m_width(width), m_height(height), m_renderer_id(0) {
	gl_call(glGenFramebuffers(1, &m_renderer_id));
}

Framebuffer::~Framebuffer() {
	gl_call(glDeleteFramebuffers(1, &m_renderer_id));
}

void Framebuffer::bind() const {
	gl_call(glBindFramebuffer(GL_FRAMEBUFFER, m_renderer_id));
}

void Framebuffer::unbind() const {
	gl_call(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void Framebuffer::attach_texture(int width, int height, int attachment, uint32_t format, int internal_format) {
	width = width == -1 ? m_width : width;
	height = height == -1 ? m_height : height;

	m_textures.emplace_back(width, height, TextureParameters{ 
		.format = format, .internal_format = internal_format
	});

	bind();
	gl_call(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, m_textures.back().m_renderer_id, 0));
	unbind();
}

void Framebuffer::attach_renderbuffer(int width, int height, int attachment, int format) {
	width = width == -1 ? m_width : width;
	height = height == -1 ? m_height : height;

	m_renderbuffers.emplace_back(width, height, format);

	bind();
	gl_call(glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, m_renderbuffers.back().m_renderer_id));
	unbind();
}

const Texture& Framebuffer::get_texture(int index) const {
	return m_textures[index];
}

const Renderbuffer& Framebuffer::get_renderbuffer(int index) const {
	return m_renderbuffers[index];
}

bool Framebuffer::is_complete() const {
	gl_call(const bool complete = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	return complete;
}

void Framebuffer::resize(int width, int height) {
	m_width = width;
	m_height = height;

	for (auto& tex : m_textures) {
		tex.resize(width, height);
	}

	for (auto& rbo : m_renderbuffers) {
		rbo.resize(width, height);
	}
}
