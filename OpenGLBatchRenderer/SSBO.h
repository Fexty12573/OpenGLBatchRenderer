#pragma once

#include <cstdint>
#include <gl/glew.h>


class SSBO {
public:
	explicit SSBO(size_t size, uint32_t usage = GL_STATIC_DRAW);
	SSBO(const SSBO&) = delete;
	SSBO(SSBO&&) noexcept;
	~SSBO();

	SSBO& operator=(const SSBO&) = delete;
	SSBO& operator=(SSBO&&) noexcept;

	void bind() const;
	void unbind() const;
	void bind_to_slot(uint32_t slot) const;

	void update(const void* data, size_t size, size_t offset = 0) const;

private:
	uint32_t m_renderer_id;
	uint32_t m_usage;
	size_t m_size;
};
