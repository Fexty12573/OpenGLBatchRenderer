#pragma once

#include <filesystem>
#include <initializer_list>
#include <vector>


class Cubemap {
public:
	/**
	 * \brief Will look for the following files in the given directory:
	 * right.jpg, left.jpg, top.jpg, bottom.jpg, front.jpg, back.jpg
	 * \param path The directory to look in
	 * \param extension The file extension to look for
	 */
	explicit Cubemap(const std::filesystem::path& path, const std::string& extension = "auto");

	/**
	 * \brief Loads the given face textures into a cubemap
	 * \param paths The paths to the textures
	 */
	explicit Cubemap(std::initializer_list<std::filesystem::path> paths);
	Cubemap(const Cubemap&) = delete;
	Cubemap(Cubemap&& cubemap) noexcept;
	~Cubemap();

	Cubemap& operator=(const Cubemap&) = delete;
	Cubemap& operator=(Cubemap&& cubemap) noexcept;

	void bind(int slot = 0) const;
	void unbind() const;

	[[nodiscard]] std::filesystem::path get_directory() const { return m_paths[0].parent_path(); }

private:
	uint32_t m_renderer_id;
	std::vector<std::filesystem::path> m_paths;
};
