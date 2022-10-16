#pragma once

#include <filesystem>
#include <type_traits>
#include <unordered_map>

#include <glm/glm.hpp>

class Shader {
public:
	Shader(std::filesystem::path vs_path, std::filesystem::path fs_path);
	Shader(const Shader& other) = delete;
	Shader(Shader&& other) noexcept;
	~Shader();

	Shader& operator=(const Shader& other) = delete;
	Shader& operator=(Shader&& other) noexcept;

	void bind() const;
	void unbind() const;

	void reload();

	void set_uniform_1i(std::string_view name, int value);
	void set_uniform_1f(std::string_view name, float value);
	void set_uniform_fv(std::string_view name, const float* value, int count);
	void set_uniform_2f(std::string_view name, float v0, float v1);
	void set_uniform_vec2(std::string_view name, const glm::vec2& value);
	void set_uniform_3f(std::string_view name, float v0, float v1, float v2);
	void set_uniform_vec3(std::string_view name, const glm::vec3& value);
	void set_uniform_4f(std::string_view name, float v0, float v1, float v2, float v3);
	void set_uniform_vec4(std::string_view name, const glm::vec4& value);
	void set_uniform_matrix_4fv(std::string_view name, const glm::mat4& value);
	void set_uniform_matrix_3fv(std::string_view name, const glm::mat3& value);

	template<class T> void set_uniform(std::string_view name, const T& value);

	void bind_ubo(const std::string& name, uint32_t binding_point) const;

private:
	[[nodiscard]] unsigned int load_shaders() const;
	[[nodiscard]] unsigned int compile_shader(std::string_view source, unsigned int type) const;
	[[nodiscard]] int get_uniform_location(std::string_view name);

private:
	std::filesystem::path m_vs_path;
	std::filesystem::path m_fs_path;

	unsigned int m_renderer_id;

	// Caching for uniforms
	std::unordered_map<std::string, int> m_uniform_location_cache;
};


template<class T> void Shader::set_uniform(std::string_view name, const T& value) {
	//typedef typename T::NotARealType X;
	if constexpr (std::is_same_v<T, int>) {
		set_uniform_1i(name, value);
	} else if constexpr (std::is_same_v<T, float>) {
		set_uniform_1f(name, value);
	} else if constexpr (std::is_same_v<T, glm::vec2>) {
		set_uniform_vec2(name, value);
	} else if constexpr (std::is_same_v<T, glm::vec3>) {
		set_uniform_vec3(name, value);
	} else if constexpr (std::is_same_v<T, glm::vec4>) {
		set_uniform_vec4(name, value);
	} else if constexpr (std::is_same_v<T, glm::mat4>) {
		set_uniform_matrix_4fv(name, value);
	} else if constexpr (std::is_same_v<T, glm::mat3>) {
		set_uniform_matrix_3fv(name, value);
	} else {
		printf("Invalid uniform type: %s\n", typeid(T).name());
	}
}
