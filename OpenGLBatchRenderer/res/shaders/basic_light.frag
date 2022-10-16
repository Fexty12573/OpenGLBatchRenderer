#version 330 core

struct Material {
	vec3 ambient;
	vec3 diffuse;
	sampler2D specular;
	float shininess;
};

struct PointLight {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
struct DirLight {
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

in vec2 v_uv;
in vec3 v_pos;
in vec3 v_normal;

out vec4 g_Color;

uniform Material u_material;
uniform PointLight u_light;

