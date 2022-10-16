#include "PointLight.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

PointLight::PointLight(const glm::vec3& position, const glm::vec3& diffuse, const glm::vec3& specular, 
	const glm::vec3& ambient, float power, float constant, float linear, float quadratic)
	: BaseLight(diffuse, specular, ambient, power, constant, linear, quadratic), m_position(position) {
}

void PointLight::upload_to_shader(Shader& shader, const std::string& name) const {
	shader.set_uniform_vec3(name + ".position", m_position);
	BaseLight::upload_to_shader(shader, name);
}

void PointLight::upload_to_light_shader(Shader& shader) const {
	auto model = glm::translate(glm::mat4(1.0f), m_position);
	model = glm::scale(model, glm::vec3(0.2f));
	shader.set_uniform_matrix_4fv("u_model", model);
	shader.set_uniform_vec3("u_diffuse", m_diffuse);
	shader.set_uniform_vec3("u_specular", m_specular);
	shader.set_uniform_vec3("u_ambient", m_ambient);
	shader.set_uniform_1f("u_power", m_power);
}

void PointLight::draw_imgui_options() {
	ImGui::PushID(this);
	ImGui::DragFloat3("Position", glm::value_ptr(m_position), 0.1f);
	ImGui::ColorEdit3("Diffuse", glm::value_ptr(m_diffuse));
	ImGui::ColorEdit3("Specular", glm::value_ptr(m_specular));
	ImGui::ColorEdit3("Ambient", glm::value_ptr(m_ambient));
	ImGui::DragFloat("Power", &m_power, 0.01f);
	ImGui::DragFloat("Constant", &m_constant, 0.01f);
	ImGui::DragFloat("Linear", &m_linear, 0.01f);
	ImGui::DragFloat("Quadratic", &m_quadratic, 0.001f, 0, 0, "%.4f");
	ImGui::PopID();
}

