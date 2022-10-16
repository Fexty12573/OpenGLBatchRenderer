#pragma once

#include <filesystem>

namespace gl {

unsigned int load_shaders(const std::filesystem::path& vertex, const std::filesystem::path& fragment);

}
