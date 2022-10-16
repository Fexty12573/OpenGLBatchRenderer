#include "VertexArray.h"

#include "Debug.h"

#include <GL/glew.h>


VertexArray::VertexArray() : m_renderer_id(0), m_attribute_index(0) {
	gl_call(glGenVertexArrays(1, &m_renderer_id));
}

VertexArray::~VertexArray() {
	gl_call(glDeleteVertexArrays(1, &m_renderer_id));
}

void VertexArray::bind() const {
	gl_call(glBindVertexArray(m_renderer_id));
}

void VertexArray::unbind() const {
	gl_call(glBindVertexArray(0));
}

void VertexArray::add_buffer(const VertexBuffer& vb, const VertexBufferLayout& layout) {
	bind();
	vb.bind();

	const auto& elements = layout.get_elements();
	uint8_t* offset = nullptr;

	for (uint32_t i = m_attribute_index; i < elements.size(); ++i, ++m_attribute_index) {
		const auto& element = elements[i];
		gl_call(glEnableVertexAttribArray(i));
		gl_call(glVertexAttribPointer(i, static_cast<int>(element.count), element.type, 
			element.normalized, static_cast<int>(layout.get_stride()), offset));

		offset += static_cast<size_t>(element.count * element.get_size());
	}

	unbind();
}

void VertexArray::add_instanced_buffer(const VertexBuffer& vb, const VertexBufferLayout& layout, uint32_t divisor) {
	bind();
	vb.bind();

	const auto& elements = layout.get_elements();
	uint8_t* offset = nullptr;

	for (auto i = m_attribute_index; i < elements.size(); ++i, ++m_attribute_index) {
		const auto& element = elements[i];
		gl_call(glEnableVertexAttribArray(i));
		gl_call(glVertexAttribPointer(i, static_cast<int>(element.count), element.type,
			element.normalized, static_cast<int>(layout.get_stride()), offset));
		gl_call(glVertexAttribDivisor(i, divisor));

		offset += static_cast<size_t>(element.count * element.get_size());
	}

	unbind();
}
