#version 330 core

in vec2 v_uv;

uniform sampler2D u_texture;

out vec4 g_Color;

void main() {
    g_Color = texture(u_texture, v_uv);
}