#include "DirectionalLight.h"


DirectionalLight::DirectionalLight(const glm::vec3& direction, const glm::vec3& diffuse, const glm::vec3& specular,
	const glm::vec3& ambient, float power, float constant, float linear, float quadratic)
	: BaseLight(diffuse, specular, ambient, power, constant, linear, quadratic), m_direction(direction) {
}

void DirectionalLight::upload_to_light_shader(Shader& shader) const {
	
}

void DirectionalLight::upload_to_shader(Shader& shader, const std::string& name) const {
	shader.set_uniform(name + ".direction", m_direction);
	BaseLight::upload_to_shader(shader, name);
}
