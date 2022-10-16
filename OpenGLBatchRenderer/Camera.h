#pragma once

#include <glm/glm.hpp>

class Camera {
public:
	Camera(float fov, float near, float far);

	void update(float dt, bool accept_mouse_input = true);
	void on_resize(int width, int height);

	// Makes the camera look at a point in world space
	void look_at(const glm::vec3& target);

	[[nodiscard]] const glm::mat4& get_projection_matrix() const { return m_projection_matrix; }
	[[nodiscard]] const glm::mat4& get_view_matrix() const { return m_view_matrix; }

	[[nodiscard]] const glm::vec3& get_position() const { return m_position; }
	[[nodiscard]] const glm::vec3& get_direction() const { return m_direction; }

private:
	void recalculate_projection_matrix();
	void recalculate_view_matrix();

private:
	glm::mat4 m_projection_matrix;
	glm::mat4 m_view_matrix;

	float m_fov;
	float m_near;
	float m_far;

	int m_width;
	int m_height;

	glm::vec3 m_position;
	glm::vec3 m_direction;
	float m_roll;
};

