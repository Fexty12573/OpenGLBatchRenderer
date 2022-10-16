#pragma once

#include <glm/glm.hpp>

#include "Shader.h"

class BaseLight {
public:
	BaseLight(
		const glm::vec3& diffuse = glm::vec3(0.5f),
		const glm::vec3& specular = glm::vec3(1.0f),
		const glm::vec3& ambient = glm::vec3(0.2f),
		float power = 1.31f, 
		float constant = 1.0f,
		float linear = 0.09f, 
		float quadratic = 0.0032f
	);
	BaseLight(const BaseLight&) = default;
	BaseLight(BaseLight&&) = default;
	virtual ~BaseLight() = default;

	BaseLight& operator=(const BaseLight&) = default;
	BaseLight& operator=(BaseLight&&) = default;

	virtual void upload_to_shader(Shader& shader, const std::string& name) const;
	virtual void upload_to_light_shader(Shader& shader) const = 0;

protected:
	glm::vec3 m_diffuse;
	glm::vec3 m_specular;
	glm::vec3 m_ambient;
	float m_power;
	float m_constant;
	float m_linear;
	float m_quadratic;
};

