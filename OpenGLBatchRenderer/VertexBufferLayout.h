#pragma once

#include <cassert>
#include <vector>
#include <GL/glew.h>

struct VertexBufferElement {
	uint32_t type;
	uint32_t count;
	uint8_t normalized;

	[[nodiscard]] constexpr uint32_t get_size() const {
		return get_size_of_type(type);
	}

	static constexpr uint32_t get_size_of_type(uint32_t type) {
		switch (type) {
		case GL_FLOAT: return sizeof(GLfloat);
		case GL_UNSIGNED_INT: return sizeof(GLuint);
		case GL_UNSIGNED_BYTE: return sizeof(GLubyte);
		default: break;
		}

		assert(false);
		return 0;
	}
};

class VertexBufferLayout {
public:
	VertexBufferLayout();

	template<class T> void push(uint32_t count);

	[[nodiscard]] uint32_t get_stride() const { return m_stride; }
	[[nodiscard]] const auto& get_elements() const { return m_elements; }

private:
	std::vector<VertexBufferElement> m_elements;
	uint32_t m_stride;
};


template<class T> void VertexBufferLayout::push(uint32_t count) {
	// Should not get instantiated with any other types
	assert(false);
}

template<> inline void VertexBufferLayout::push<float>(uint32_t count) {
	m_elements.push_back({ GL_FLOAT, count, GL_FALSE });
	m_stride += m_elements.back().get_size() * count;
}

template<> inline void VertexBufferLayout::push<uint32_t>(uint32_t count) {
	m_elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
	m_stride += m_elements.back().get_size() * count;
}

template<> inline void VertexBufferLayout::push<uint8_t>(uint32_t count) {
	m_elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE });
	m_stride += m_elements.back().get_size() * count;
}
