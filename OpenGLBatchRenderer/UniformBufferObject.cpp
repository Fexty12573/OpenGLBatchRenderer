#include "UniformBufferObject.h"
#include "Debug.h"


UniformBufferObject::UniformBufferObject(uint32_t size, const void* data, uint32_t usage) : m_renderer_id(0), m_size(size), m_usage(usage) {
	gl_call(glGenBuffers(1, &m_renderer_id));
	gl_call(glBindBuffer(GL_UNIFORM_BUFFER, m_renderer_id));
	gl_call(glBufferData(GL_UNIFORM_BUFFER, m_size, data, m_usage));
}

UniformBufferObject::UniformBufferObject(UniformBufferObject&& ubo) noexcept {
	gl_call(glDeleteBuffers(1, &m_renderer_id));

	m_renderer_id = ubo.m_renderer_id;
	m_size = ubo.m_size;
	m_usage = ubo.m_usage;

	ubo.m_renderer_id = 0;
	ubo.m_size = 0;
	ubo.m_usage = 0;
}

UniformBufferObject::~UniformBufferObject() {
	gl_call(glDeleteBuffers(1, &m_renderer_id));
}

UniformBufferObject& UniformBufferObject::operator=(UniformBufferObject&& ubo) noexcept {
	if (this != &ubo) {
		gl_call(glDeleteBuffers(1, &m_renderer_id));

		m_renderer_id = ubo.m_renderer_id;
		m_size = ubo.m_size;
		m_usage = ubo.m_usage;

		ubo.m_renderer_id = 0;
		ubo.m_size = 0;
		ubo.m_usage = 0;
	}

	return *this;
}

void UniformBufferObject::bind_to_slot(uint32_t binding_point) const {
	gl_call(glBindBufferBase(GL_UNIFORM_BUFFER, binding_point, m_renderer_id));
}

void UniformBufferObject::update(const void* data, uint32_t size, uint32_t offset) const {
	gl_call(glBindBuffer(GL_UNIFORM_BUFFER, m_renderer_id));
	gl_call(glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data));
	gl_call(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}

void UniformBufferObject::update(const void* data) const {
	gl_call(glBindBuffer(GL_UNIFORM_BUFFER, m_renderer_id));
	gl_call(glBufferSubData(GL_UNIFORM_BUFFER, 0, m_size, data));
	gl_call(glBindBuffer(GL_UNIFORM_BUFFER, 0));
}
