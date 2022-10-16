#version 430 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec2 a_uv;
layout (location = 2) in vec3 a_normal;

layout (std140) uniform Camera {
    mat4 u_view;
	mat4 u_projection;
	mat4 u_viewproj;
};

uniform mat3 u_normal_matrix;
uniform mat4 u_model;
//uniform mat4 u_view;
//uniform mat4 u_projection;
uniform mat4 u_mvp;

out VS_OUT {
    vec2 uv;
    vec3 ws_position; // ws = world space
    vec3 ws_normal;
} vs_out;


void main() {
    gl_Position = u_mvp * vec4(a_position, 1.0);

    vs_out.uv = a_uv;

    vs_out.ws_position = (u_model * vec4(a_position, 1.0)).xyz;
    vs_out.ws_normal = normalize(u_normal_matrix * a_normal);
}
