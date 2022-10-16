#include "SSBO.h"

#include "Debug.h"

SSBO::SSBO(size_t size, uint32_t usage) : m_renderer_id(0), m_usage(usage), m_size(size) {
	gl_call(glGenBuffers(1, &m_renderer_id));
	gl_call(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_renderer_id));
	gl_call(glBufferData(GL_SHADER_STORAGE_BUFFER, static_cast<GLsizeiptr>(size), nullptr, usage));
	gl_call(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0));
}

SSBO::SSBO(SSBO&& ssbo) noexcept {
	gl_call(glDeleteBuffers(1, &m_renderer_id));

	m_renderer_id = ssbo.m_renderer_id;
	m_usage = ssbo.m_usage;
	m_size = ssbo.m_size;

	ssbo.m_renderer_id = 0;
	ssbo.m_usage = 0;
	ssbo.m_size = 0;
}

SSBO::~SSBO() {
	gl_call(glDeleteBuffers(1, &m_renderer_id));
}

SSBO& SSBO::operator=(SSBO&& ssbo) noexcept {
	if (this != &ssbo) {
		gl_call(glDeleteBuffers(1, &m_renderer_id));

		m_renderer_id = ssbo.m_renderer_id;
		m_usage = ssbo.m_usage;
		m_size = ssbo.m_size;

		ssbo.m_renderer_id = 0;
		ssbo.m_usage = 0;
		ssbo.m_size = 0;
	}

	return *this;
}

void SSBO::bind() const {
	gl_call(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_renderer_id));
}

void SSBO::unbind() const {
	gl_call(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0));
}

void SSBO::bind_to_slot(uint32_t slot) const {
	gl_call(glBindBufferBase(GL_SHADER_STORAGE_BUFFER, slot, m_renderer_id));
}

void SSBO::update(const void* data, size_t size, size_t offset) const {
	gl_call(glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_renderer_id));
	gl_call(glBufferSubData(GL_SHADER_STORAGE_BUFFER, static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(size), data));
	gl_call(glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0));
}

