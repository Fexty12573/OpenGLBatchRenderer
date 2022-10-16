#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 transform;
out vec3 frag_pos;

void main() {
    frag_pos = position;
    gl_Position = transform * vec4(position, 1);
}
