#pragma once

#include "BaseLight.h"


class DirectionalLight : public BaseLight {
public:
	explicit DirectionalLight(
		const glm::vec3& direction,
		const glm::vec3& diffuse = glm::vec3(0.5f),
		const glm::vec3& specular = glm::vec3(1.0f),
		const glm::vec3& ambient = glm::vec3(0.2f),
		float power = 1.31f,
		float constant = 1.0f,
		float linear = 0.09f,
		float quadratic = 0.0032f
	);

	DirectionalLight(const DirectionalLight&) = default;
	DirectionalLight(DirectionalLight&&) = default;
	~DirectionalLight() override = default;

	DirectionalLight& operator=(const DirectionalLight&) = default;
	DirectionalLight& operator=(DirectionalLight&&) = default;

	void upload_to_light_shader(Shader& shader) const override;
	void upload_to_shader(Shader& shader, const std::string& name) const override;

	friend class Renderer;

private:
	glm::vec3 m_direction;
};
