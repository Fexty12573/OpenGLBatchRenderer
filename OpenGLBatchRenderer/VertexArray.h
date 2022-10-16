#pragma once

#include <cstdint>

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

class VertexArray {
public:
	VertexArray();
	~VertexArray();

	void bind() const;
	void unbind() const;

	void add_buffer(const VertexBuffer& vb, const VertexBufferLayout& layout);
	void add_instanced_buffer(const VertexBuffer& vb, const VertexBufferLayout& layout, uint32_t divisor);

private:
	uint32_t m_renderer_id;

	uint32_t m_attribute_index;
};

