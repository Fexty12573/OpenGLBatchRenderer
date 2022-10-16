#include "Renderbuffer.h"

#include "Debug.h"

Renderbuffer::Renderbuffer(int width, int height, int internal_format)
	: m_renderer_id(0), m_width(width), m_height(height), m_format(internal_format) {
	gl_call(glGenRenderbuffers(1, &m_renderer_id));

	gl_call(glBindBuffer(GL_RENDERBUFFER, m_renderer_id));
	gl_call(glRenderbufferStorage(GL_RENDERBUFFER, m_format, m_width, m_height));
	gl_call(glBindBuffer(GL_RENDERBUFFER, 0));
}

Renderbuffer::Renderbuffer(Renderbuffer&& other) noexcept
	: m_width(other.m_width), m_height(other.m_height), m_format(other.m_format) {

	m_renderer_id = other.m_renderer_id;
	other.m_renderer_id = 0;
}

Renderbuffer& Renderbuffer::operator=(Renderbuffer&& other) noexcept {
	if (this != &other) {
		m_width = other.m_width;
		m_height = other.m_height;
		m_format = other.m_format;

		m_renderer_id = other.m_renderer_id;
		other.m_renderer_id = 0;
	}

	return *this;
}

Renderbuffer::~Renderbuffer() {
	gl_call(glDeleteRenderbuffers(1, &m_renderer_id));
}

void Renderbuffer::bind() const {
	gl_call(glBindRenderbuffer(GL_RENDERBUFFER, m_renderer_id));
}

void Renderbuffer::unbind() const {
	gl_call(glBindRenderbuffer(GL_RENDERBUFFER, 0));
}

void Renderbuffer::resize(int width, int height) {
	m_width = width;
	m_height = height;
	bind();
	gl_call(glRenderbufferStorage(GL_RENDERBUFFER, m_format, m_width, m_height));
	unbind();
}
