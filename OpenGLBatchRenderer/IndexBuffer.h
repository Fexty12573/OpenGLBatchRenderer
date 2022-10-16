#pragma once

#include <gl/glew.h>

class IndexBuffer {
public:
	IndexBuffer(const unsigned int* data, unsigned int count, unsigned int usage = GL_STATIC_DRAW);
	IndexBuffer(const IndexBuffer&) = delete;
	IndexBuffer(IndexBuffer&& ib) noexcept;
	~IndexBuffer();

	IndexBuffer& operator=(const IndexBuffer&) = delete;
	IndexBuffer& operator=(IndexBuffer&& ib) noexcept;

	void bind() const;
	void unbind() const;

	void update(const unsigned int* data, unsigned int count);

	[[nodiscard]] unsigned int count() const { return m_count; }

private:
	unsigned int m_renderer_id;
	unsigned int m_count;
	unsigned int m_usage;
};

