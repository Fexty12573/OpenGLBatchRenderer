#include "VertexBuffer.h"

#include "Debug.h"

#include <GL/glew.h>

VertexBuffer::VertexBuffer(const void* data, unsigned int size, unsigned int usage) : m_renderer_id(0), m_size(size), m_usage(usage) {
	gl_call(glGenBuffers(1, &m_renderer_id));
	gl_call(glBindBuffer(GL_ARRAY_BUFFER, m_renderer_id));
	gl_call(glBufferData(GL_ARRAY_BUFFER, m_size, data, m_usage));
}

VertexBuffer::VertexBuffer(VertexBuffer&& other) noexcept {
	gl_call(glDeleteBuffers(1, &m_renderer_id));

	m_renderer_id = other.m_renderer_id;
	m_size = other.m_size;
	m_usage = other.m_usage;

	other.m_renderer_id = 0;
	other.m_size = 0;
	other.m_usage = 0;
}

VertexBuffer& VertexBuffer::operator=(VertexBuffer&& other) noexcept {
	if (this != &other) {
		gl_call(glDeleteBuffers(1, &m_renderer_id));

		m_renderer_id = other.m_renderer_id;
		m_size = other.m_size;
		m_usage = other.m_usage;

		other.m_renderer_id = 0;
		other.m_size = 0;
		other.m_usage = 0;
	}

	return *this;
}

VertexBuffer::~VertexBuffer() {
	gl_call(glDeleteBuffers(1, &m_renderer_id));
}

void VertexBuffer::update(const void* data) {
	gl_call(glBindBuffer(GL_ARRAY_BUFFER, m_renderer_id));
	gl_call(glBufferSubData(GL_ARRAY_BUFFER, 0, m_size, data));
	gl_call(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void VertexBuffer::update(const void* data, unsigned int size, unsigned int offset) {
	gl_call(glBindBuffer(GL_ARRAY_BUFFER, m_renderer_id));
	gl_call(glBufferSubData(GL_ARRAY_BUFFER, offset, size, data));
	gl_call(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void VertexBuffer::bind() const {
	gl_call(glBindBuffer(GL_ARRAY_BUFFER, m_renderer_id));
}

void VertexBuffer::unbind() const {
	gl_call(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
