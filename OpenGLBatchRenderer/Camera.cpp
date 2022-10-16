#include "Camera.h"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <GLFW/glfw3.h>



Camera::Camera(float fov, float near, float far) : m_fov(fov), m_near(near), m_far(far) {
	glfwGetWindowSize(glfwGetCurrentContext(), &m_width, &m_height);
	recalculate_projection_matrix();

	m_position = { 4, 3, 3 };
	m_direction = { 0, 0, -1 };
}

void Camera::update(float dt, bool accept_mouse_input) {
	const auto window = glfwGetCurrentContext();
	constexpr float sensitivity = 0.001f;

	const glm::vec<2, double> screen_middle = { m_width / 2.0, m_height / 2.0 };
	glm::vec<2, double> mouse_pos{};
	glfwGetCursorPos(window, &mouse_pos.x, &mouse_pos.y);
	const glm::vec2 delta = mouse_pos - screen_middle;

	glfwSetCursorPos(window, screen_middle.x, screen_middle.y);

	constexpr glm::vec3 up = { 0, 1, 0 };
	const glm::vec3 right = glm::cross(m_direction, up);

	const float speed = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) ? 6.0f : 3.0f;
	bool moved = false;

	if (glfwGetKey(window, GLFW_KEY_W)) {
		m_position += m_direction * speed * dt;
		moved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_S)) {
		m_position -= m_direction * speed * dt;
		moved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_A)) {
		m_position -= right * speed * dt;
		moved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_D)) {
		m_position += right * speed * dt;
		moved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE)) {
		m_position += up * speed * dt;
		moved = true;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) {
		m_position -= up * speed * dt;
		moved = true;
	}

	// Unused for now
	if (glfwGetKey(window, GLFW_KEY_Q)) {
		m_roll += 1.0f * dt;
	} else if (glfwGetKey(window, GLFW_KEY_E)) {
		m_roll -= 1.0f * dt;
	}

	if (!accept_mouse_input) {
		return;
	}

	if (any(epsilonNotEqual(delta, glm::vec2{ 0, 0 }, 0.01f))) {
		const float pitch_delta = delta.y * sensitivity;
		const float yaw_delta = delta.x * sensitivity;

		const float collinearity = glm::dot(m_direction, up);
		if ((collinearity < 0.98f || pitch_delta > 0.0f) && (collinearity > -0.98f || pitch_delta < 0.0f)) {
			const glm::quat q = glm::normalize(glm::cross(glm::angleAxis(-pitch_delta, right),
				glm::angleAxis(-yaw_delta, up)));
			m_direction = glm::rotate(q, m_direction);

			moved = true;
		}
	}

	if (moved) {
		recalculate_view_matrix();
	}
}

void Camera::on_resize(int width, int height) {
	m_width = width;
	m_height = height;
	recalculate_projection_matrix();
}

void Camera::look_at(const glm::vec3& target) {
	m_direction = glm::normalize(target - m_position);
}

void Camera::recalculate_projection_matrix() {
	const auto w = static_cast<float>(m_width);
	const auto h = static_cast<float>(m_height);

	std::printf("Updating camera projection matrix\n");
	m_projection_matrix = glm::perspectiveFov(m_fov, w, h, m_near, m_far);
}

void Camera::recalculate_view_matrix() {
	//const glm::vec3 up = glm::rotateZ(glm::vec3{ 0, 1, 0 }, m_roll);
	constexpr glm::vec3 up = { 0, 1, 0 };
	m_view_matrix = glm::lookAt(m_position, m_position + m_direction, up);
}
