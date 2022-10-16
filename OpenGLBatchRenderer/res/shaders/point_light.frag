#version 330 core

uniform vec3 u_ambient;
uniform vec3 u_diffuse;
uniform vec3 u_specular;
uniform float u_power;

const float ambient_weight = 0.2;
const float diffuse_weight = 0.5;
const float specular_weight = 0.3;

out vec4 g_Color;

void main() {
    vec3 color = u_ambient * ambient_weight;
    color += u_diffuse * diffuse_weight;
    color += u_specular * specular_weight;
    g_Color = vec4(u_diffuse * u_power, 1.0);
}
