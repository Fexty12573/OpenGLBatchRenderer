#pragma once
#include "BaseLight.h"

class PointLight final : public BaseLight {
public:
	explicit PointLight(
		const glm::vec3& position,
		const glm::vec3& diffuse = glm::vec3(0.5f),
		const glm::vec3& specular = glm::vec3(1.0f),
		const glm::vec3& ambient = glm::vec3(0.2f),
		float power = 1.31f,
		float constant = 1.0f,
		float linear = 0.09f,
		float quadratic = 0.0032f
	);
	PointLight(const PointLight&) = default;
	PointLight(PointLight&&) = default;
	~PointLight() override = default;

	PointLight& operator=(const PointLight&) = default;
	PointLight& operator=(PointLight&&) = default;

	/**
	 * \brief Uploads the light data to the shader. The shader must contain a struct with the following members:
	 * position, diffuse, specular, ambient, power, constant, linear, quadratic
	 * \param shader The shader to upload the data to.
	 * \param name The name of the struct in the shader.
	 */
	void upload_to_shader(Shader& shader, const std::string& name) const override;

	/**
	 * \brief Uploads the light data to a lamp shader. The shader must contain the following uniforms:
	 * u_model, u_diffuse, u_specular, u_ambient, u_power
	 * \param shader The shader to upload the data to.
	 */
	void upload_to_light_shader(Shader& shader) const override;

	void draw_imgui_options();

	friend class Renderer;

private:
	glm::vec3 m_position;
};

