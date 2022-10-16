#include "Shaders.h"

#include <fstream>
#include <string>

#include <fmt/format.h>
#include <GL/glew.h>

namespace fs = std::filesystem;

unsigned int compile_shader(std::string_view source, uint32_t type);

unsigned int gl::load_shaders(const fs::path& vertex, const fs::path& fragment) {
	const auto program = glCreateProgram();

	std::ifstream ifs{ vertex };
	const std::string vertex_source(std::istreambuf_iterator{ ifs }, {});

	ifs.close();
	ifs.open(fragment);
	const std::string fragment_source(std::istreambuf_iterator{ ifs }, {});

	const auto vs_id = compile_shader(vertex_source, GL_VERTEX_SHADER);
	const auto fs_id = compile_shader(fragment_source, GL_FRAGMENT_SHADER);

	glAttachShader(program, vs_id);
	glAttachShader(program, fs_id);
	glLinkProgram(program);

	{
		int status = 0;
		int log_len = 0;

		glGetProgramiv(program, GL_COMPILE_STATUS, &status);
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_len);
		if (log_len > 0) {
			std::string log(log_len + 1, '\0');
			glGetProgramInfoLog(program, static_cast<int>(log.size()), &log_len, log.data());
			fmt::print("Failed to compile shader: {}\n", log);
		}
	}

	glDetachShader(program, vs_id);
	glDetachShader(program, fs_id);

	glDeleteShader(vs_id);
	glDeleteShader(fs_id);

	return program;
}

unsigned int compile_shader(std::string_view source, uint32_t type) {
	const auto id = glCreateShader(type);
	const char* const src_buf = source.data();
	const int length = static_cast<int>(source.size());
	int status = 0;
	int log_len = 0;

	glShaderSource(id, 1, &src_buf, &length);
	glGetShaderiv(id, GL_COMPILE_STATUS, &status);
	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &log_len);
	if (log_len > 0) {
		std::string log(log_len + 1, '\0');
		glGetShaderInfoLog(id, static_cast<int>(log.size()), &log_len, log.data());
		fmt::print("Failed to compile shader: {}\n", log);
	}

	return id;
}
