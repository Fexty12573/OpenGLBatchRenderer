#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;

uniform mat4 transform;
uniform mat4 model; // Used in fragment shader
uniform mat4 view;
uniform mat4 projection;

uniform vec3 light_pos;

out vec3 frag_pos;
out vec3 frag_normal;

void main() {

	frag_pos = (model * vec4(position, 1.0)).xyz;
	frag_normal = mat3(model) * normal;
	
	gl_Position = transform * vec4(position, 1.0);
}
