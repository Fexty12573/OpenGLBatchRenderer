#version 330 core

uniform samplerCube u_skybox;

in vec3 v_position;

out vec4 g_Color;

void main() {
	g_Color = vec4(texture(u_skybox, v_position).rgb, 1.0);
}