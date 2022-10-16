#pragma once
#include <GL/glew.h>

class VertexBuffer {
public:
	VertexBuffer(const void* data, unsigned int size, unsigned int usage = GL_STATIC_DRAW);
	VertexBuffer(const VertexBuffer& other) = delete;
	VertexBuffer(VertexBuffer&& other) noexcept;
	~VertexBuffer();

	VertexBuffer& operator=(const VertexBuffer& other) = delete;
	VertexBuffer& operator=(VertexBuffer&& other) noexcept;

	void update(const void* data);
	void update(const void* data, unsigned int size, unsigned int offset = 0);

	void bind() const;
	void unbind() const;

private:
	unsigned int m_renderer_id;
	unsigned int m_size;
	unsigned int m_usage;
};

