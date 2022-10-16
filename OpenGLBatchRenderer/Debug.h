#pragma once

#include <fmt/format.h>
#include <GL/glew.h>

#define GL_ERROR_CHECKING 1

#if defined(_DEBUG)
#define gl_assert(x) if (!(x)) __debugbreak()
#else
#define gl_assert(x) x
#endif

#if GL_ERROR_CHECKING
#define gl_call(x) gl_clear_error();\
	x;\
	gl_assert(gl_log_call(#x, __FILE__, __LINE__))
#else
#define gl_call(x) x
#endif

#define gl_error(msg, ...) log_error(msg, __VA_ARGS__)

inline void gl_clear_error() {
	while (glGetError() != GL_NO_ERROR);
}

inline bool gl_log_call(const char* function, const char* file, int line) {
	if (GLenum error = glGetError()) {
		fmt::print("[OpenGL Error] 0x{:04X} caused by {} at {} ({})\n", error, function, file, line);
		return false;
	}

	return true;
}

template<typename ...T>
void log_error(const fmt::format_string<T...>& message, T... args) {
	fmt::print("[Error] {}\n", fmt::format(message, std::forward<T...>(args)...));
}
