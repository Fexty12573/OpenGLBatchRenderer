#version 330 core

uniform vec4 axis_color;

layout(location = 0) out vec4 color;

void main() {
    color = axis_color;
}