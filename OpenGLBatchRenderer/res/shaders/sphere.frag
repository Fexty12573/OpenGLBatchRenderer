#version 330 core

uniform mat4 transform;
uniform mat4 model; // Used in fragment shader
uniform mat4 view;
uniform mat4 projection;

uniform vec3 light_pos;
uniform vec3 light_color;
uniform float light_power;
uniform float specular_power;
uniform vec3 camera_pos;

in vec3 frag_pos;
in vec3 frag_normal;

out vec4 gl_Color;

float calculate_diffuse_light(vec3 normal) {
	vec3 light_direction = normalize(light_pos - frag_pos);
	float dist = length(light_direction);
	float dist_sq = dist * dist;
	
	float diffuse = max(dot(light_direction, normal), 0.0) / dist_sq;

	return diffuse;
}

float calculate_specular_light(vec3 normal) {
	vec3 L = normalize(frag_pos - light_pos);
	vec3 R = reflect(L, normal);
	vec3 V = normalize(camera_pos - frag_pos);
	
	float specular = pow(max(dot(R, V), 0.0), specular_power);

	return specular;
}

void main() {
	vec3 normal = normalize(frag_normal);
	
	vec4 ambient_color = vec4(0.1) * vec4(light_color, 1.0);
	float diffuse = calculate_diffuse_light(normal);
	float specular = calculate_specular_light(normal);
	
	
	gl_Color = ambient_color + vec4(light_color * light_power * diffuse, 1.0) + vec4(light_color * light_power * specular, 1.0);
}
 