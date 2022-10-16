#include "BaseLight.h"

BaseLight::BaseLight(const glm::vec3& diffuse, const glm::vec3& specular, const glm::vec3& ambient, float power, float constant, float linear, float quadratic)
	: m_diffuse(diffuse), m_specular(specular), m_ambient(ambient), m_power(power), m_constant(constant), m_linear(linear), m_quadratic(quadratic) {
}

void BaseLight::upload_to_shader(Shader& shader, const std::string& name) const {
	shader.set_uniform(name + ".diffuse", m_diffuse);
	shader.set_uniform(name + ".specular", m_specular);
	shader.set_uniform(name + ".ambient", m_ambient);
	shader.set_uniform(name + ".power", m_power);
	shader.set_uniform(name + ".constant", m_constant);
	shader.set_uniform(name + ".linear", m_linear);
	shader.set_uniform(name + ".quadratic", m_quadratic);
}
