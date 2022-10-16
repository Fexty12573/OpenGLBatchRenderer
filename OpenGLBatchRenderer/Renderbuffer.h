#pragma once

#include <cstdint>
#include <gl/glew.h>

class Framebuffer;

class Renderbuffer {
public:
	Renderbuffer(int width, int height, int internal_format = GL_DEPTH_COMPONENT);
	Renderbuffer(const Renderbuffer&) = delete;
	Renderbuffer(Renderbuffer&& other) noexcept;
	~Renderbuffer();

	Renderbuffer& operator=(const Renderbuffer&) = delete;
	Renderbuffer& operator=(Renderbuffer&& other) noexcept;

	void bind() const;
	void unbind() const;

	void resize(int width, int height);

	friend class Framebuffer;

private:
	uint32_t m_renderer_id;
	int m_width;
	int m_height;
	int m_format;
};

