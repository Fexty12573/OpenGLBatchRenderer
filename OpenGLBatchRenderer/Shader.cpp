#include "Shader.h"

#include "Debug.h"

#include <fstream>
#include <GL/glew.h>

Shader::Shader(std::filesystem::path vs_path, std::filesystem::path fs_path)
	: m_vs_path(std::move(vs_path)), m_fs_path(std::move(fs_path)), m_renderer_id(0) {
	m_renderer_id = load_shaders();
}

Shader::Shader(Shader&& other) noexcept
	: m_vs_path(std::move(other.m_vs_path))
	, m_fs_path(std::move(other.m_fs_path))
	, m_renderer_id(other.m_renderer_id)
	, m_uniform_location_cache(std::move(other.m_uniform_location_cache)) {
	other.m_renderer_id = 0;
}

Shader::~Shader() {
	gl_call(glDeleteProgram(m_renderer_id));
}

Shader& Shader::operator=(Shader&& other) noexcept {
	if (this != &other) {
		m_vs_path = std::move(other.m_vs_path);
		m_fs_path = std::move(other.m_fs_path);
		m_uniform_location_cache = std::move(other.m_uniform_location_cache);
		m_renderer_id = other.m_renderer_id;

		other.m_renderer_id = 0;
	}

	return *this;
}

void Shader::bind() const {
	gl_call(glUseProgram(m_renderer_id));
}

void Shader::unbind() const {
	gl_call(glUseProgram(0));
}

void Shader::reload() {
	gl_call(glDeleteProgram(m_renderer_id));
	m_uniform_location_cache.clear();
	m_renderer_id = load_shaders();
}

void Shader::set_uniform_1i(std::string_view name, int value) {
	gl_call(glUniform1i(get_uniform_location(name), value));
}

void Shader::set_uniform_1f(std::string_view name, float value) {
	gl_call(glUniform1f(get_uniform_location(name), value));
}

void Shader::set_uniform_fv(std::string_view name, const float* value, int count) {
	gl_call(glUniform1fv(get_uniform_location(name), count, value));
}

void Shader::set_uniform_2f(std::string_view name, float v0, float v1) {
	gl_call(glUniform2f(get_uniform_location(name), v0, v1));
}

void Shader::set_uniform_vec2(std::string_view name, const glm::vec2& value) {
	gl_call(glUniform2f(get_uniform_location(name), value.x, value.y));
}

void Shader::set_uniform_3f(std::string_view name, float v0, float v1, float v2) {
	gl_call(glUniform3f(get_uniform_location(name), v0, v1, v2));
}

void Shader::set_uniform_vec3(std::string_view name, const glm::vec3& value) {
	gl_call(glUniform3fv(get_uniform_location(name), 1, &value[0]));
}

void Shader::set_uniform_4f(std::string_view name, float v0, float v1, float v2, float v3) {
	gl_call(glUniform4f(get_uniform_location(name), v0, v1, v2, v3));
}

void Shader::set_uniform_vec4(std::string_view name, const glm::vec4& value) {
	gl_call(glUniform4fv(get_uniform_location(name), 1, &value[0]));
}

void Shader::set_uniform_matrix_4fv(std::string_view name, const glm::mat4& value) {
	gl_call(glUniformMatrix4fv(get_uniform_location(name), 1, false, &value[0][0]));
}

void Shader::set_uniform_matrix_3fv(std::string_view name, const glm::mat3& value) {
	gl_call(glUniformMatrix3fv(get_uniform_location(name), 1, false, &value[0][0]));
}

void Shader::bind_ubo(const std::string& name, uint32_t binding_point) const {
	gl_call(const uint32_t index = glGetUniformBlockIndex(m_renderer_id, name.c_str()));
	gl_call(glUniformBlockBinding(m_renderer_id, index, binding_point));
}

unsigned int Shader::load_shaders() const {
	if (!std::filesystem::exists(m_vs_path) || !std::filesystem::exists(m_fs_path)) {
		throw std::runtime_error("Shader file does not exist");
	}

	const auto program = glCreateProgram();

	std::ifstream ifs{ m_vs_path };
	const std::string vertex_source(std::istreambuf_iterator{ ifs }, {});

	ifs.close();
	ifs.open(m_fs_path);
	const std::string fragment_source(std::istreambuf_iterator{ ifs }, {});

	const auto vs_id = compile_shader(vertex_source, GL_VERTEX_SHADER);
	const auto fs_id = compile_shader(fragment_source, GL_FRAGMENT_SHADER);

	gl_call(glAttachShader(program, vs_id));
	gl_call(glAttachShader(program, fs_id));
	gl_call(glLinkProgram(program));

	gl_call(glDetachShader(program, vs_id));
	gl_call(glDetachShader(program, fs_id));

	gl_call(glDeleteShader(vs_id));
	gl_call(glDeleteShader(fs_id));

	return program;
}

unsigned int Shader::compile_shader(std::string_view source, unsigned int type) const {
	gl_call(const auto id = glCreateShader(type));

	const char* const src_buf = source.data();
	const int length = static_cast<int>(source.size());

	gl_call(glShaderSource(id, 1, &src_buf, &length));

	return id;
}

int Shader::get_uniform_location(std::string_view name) {
	const std::string _name(name);

	const auto cached = m_uniform_location_cache.find(_name);
	if (cached != m_uniform_location_cache.end()) {
		return cached->second;
	}

	gl_call(const int location = glGetUniformLocation(m_renderer_id, name.data()));
	m_uniform_location_cache[_name] = location;

	return location;
}
