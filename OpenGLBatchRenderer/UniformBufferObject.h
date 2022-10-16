#pragma once

#include <cstdint>
#include <GL/glew.h>


class UniformBufferObject {
public:
	explicit UniformBufferObject(uint32_t size, const void* data = nullptr, uint32_t usage = GL_DYNAMIC_DRAW);
	UniformBufferObject(const UniformBufferObject&) = delete;
	UniformBufferObject(UniformBufferObject&& ubo) noexcept;
	~UniformBufferObject();

	UniformBufferObject& operator=(const UniformBufferObject&) = delete;
	UniformBufferObject& operator=(UniformBufferObject&& ubo) noexcept;

	void bind_to_slot(uint32_t binding_point) const;

	void update(const void* data, uint32_t size, uint32_t offset = 0) const;
	void update(const void* data) const;

	[[nodiscard]] uint32_t get_id() const { return m_renderer_id; }
	[[nodiscard]] uint32_t get_size() const { return m_size; }
	[[nodiscard]] uint32_t get_usage() const { return m_usage; }

private:
	uint32_t m_renderer_id;
	uint32_t m_size;
	uint32_t m_usage;
};
