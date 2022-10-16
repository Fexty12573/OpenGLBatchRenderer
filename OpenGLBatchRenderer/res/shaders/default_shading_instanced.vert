#version 430 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_TexCoords;
layout (location = 3) in float a_MaterialIndex;
layout (location = 4) in float a_ModelIndex;

layout (std140) uniform Camera {
    mat4 u_View;
	mat4 u_Projection;
	mat4 u_ViewProj;
};

layout (std430, binding = 0) buffer ModelMatrices {
    mat4 u_ModelMatrices[];
};

out VS_OUT {
    vec2 TexCoords;
    vec3 Position; // ws = world space
    vec3 Normal;
	float MaterialIndex;
} vs_out;


void main() {
    int index = int(a_ModelIndex);
    gl_Position = u_ViewProj * u_ModelMatrices[index] * vec4(a_Position, 1.0);

    vs_out.TexCoords = a_TexCoords;
    vs_out.Position = (u_ModelMatrices[index] * vec4(a_Position, 1.0)).xyz;
	vs_out.Normal = normalize(mat3(u_ModelMatrices[index + 1]) * a_Normal);
	vs_out.MaterialIndex = a_MaterialIndex;
}
