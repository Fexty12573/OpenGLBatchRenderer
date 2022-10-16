#include "IndexBuffer.h"

#include <GL/glew.h>

#include "Debug.h"

IndexBuffer::IndexBuffer(const unsigned int* data, unsigned int count, unsigned int usage) : m_count(count), m_usage(usage) {
	gl_call(glGenBuffers(1, &m_renderer_id));
	gl_call(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_renderer_id));
	gl_call(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, m_usage));
}

IndexBuffer::IndexBuffer(IndexBuffer&& ib) noexcept {
	gl_call(glDeleteBuffers(1, &m_renderer_id));

	m_renderer_id = ib.m_renderer_id;
	m_count = ib.m_count;

	ib.m_renderer_id = 0;
	ib.m_count = 0;
}

IndexBuffer& IndexBuffer::operator=(IndexBuffer&& ib) noexcept {
	if (this != &ib) {
		gl_call(glDeleteBuffers(1, &m_renderer_id));

		m_renderer_id = ib.m_renderer_id;
		m_count = ib.m_count;

		ib.m_renderer_id = 0;
		ib.m_count = 0;
	}

	return *this;
}

IndexBuffer::~IndexBuffer() {
	gl_call(glDeleteBuffers(1, &m_renderer_id));
}

void IndexBuffer::bind() const {
	gl_call(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_renderer_id));
}

void IndexBuffer::unbind() const {
	gl_call(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void IndexBuffer::update(const unsigned int* data, unsigned int count) {
	gl_call(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_renderer_id));
	gl_call(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, count * sizeof(unsigned int), data));
	gl_call(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
