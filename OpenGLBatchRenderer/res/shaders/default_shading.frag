#version 430 core

struct Material {
    sampler2D diffuse;  // diffuse color taken from diffuse map
    sampler2D specular; // specular color taken from specular map
    float shininess;
};
struct SampledMaterial {
    vec3 diffuse;  // diffuse color taken from diffuse map
    vec3 specular; // specular color taken from specular map
    float shininess;
};

struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float power;
    float constant;
    float linear;
    float quadratic;
};

struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float power;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cut_off;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float power;
    float constant;
    float linear;
    float quadratic;
};

// Constants
#define MAX_POINT_LIGHTS 4
#define MAX_MATERIALS 1

// Uniforms
uniform vec3 u_view_pos;
uniform Material u_materials[MAX_MATERIALS];
uniform PointLight u_lights[MAX_POINT_LIGHTS];
uniform DirectionalLight u_dir_light;
uniform SpotLight u_spot_light;
uniform float u_time;
uniform samplerCube u_skybox;

// Vertex shader inputs
in VS_OUT {
    vec2 uv;
    vec3 ws_position;
    vec3 ws_normal;
} fs_in;

out vec4 g_Color;

vec3 g_normal = vec3(0.0);

vec3 calculate_point_light(PointLight light, SampledMaterial mat);
vec3 calculate_directional_light(DirectionalLight light, SampledMaterial mat);
vec3 calculate_spot_light(SpotLight light, SampledMaterial mat);

void main() {
    g_normal = normalize(fs_in.ws_normal);
    
    vec3 mat_ambient_diffuse = texture(u_materials[0].diffuse, fs_in.uv).rgb;
    vec3 mat_specular = texture(u_materials[0].specular, fs_in.uv).rgb;
    SampledMaterial mat = SampledMaterial(mat_ambient_diffuse, mat_specular, u_materials[0].shininess);

    vec3 final_color = vec3(0.0);

    final_color += calculate_directional_light(u_dir_light, mat);
    for (int i = 0; i < MAX_POINT_LIGHTS; i++) {
        final_color += calculate_point_light(u_lights[i], mat);
    }

	vec3 L = normalize(u_lights[0].position - fs_in.ws_position);
#if 0
    if (gl_FragCoord.x < 960) {
        const float ratio = 1.0 / 1.52;
        vec3 V = normalize(fs_in.ws_position - u_view_pos);
        vec3 R = refract(V, g_normal, ratio);

	    final_color = texture(u_skybox, R).rgb;
    }
#endif

    g_Color = vec4(final_color, 1.0);
}


float calculate_diffuse_component(vec3 light_dir, vec3 normal) {
    float diffuse = max(dot(normal, normalize(light_dir)), 0.0);
    return diffuse;
}

float calculate_specular_component(vec3 light_dir, vec3 normal, float shininess) {
    vec3 L = normalize(light_dir);
    vec3 N = normal;

    vec3 R = reflect(-L, N);
    vec3 V = normalize(u_view_pos - fs_in.ws_position);
    return pow(max(dot(R, V), 0.0), shininess);
}

vec3 calculate_point_light(PointLight light, SampledMaterial mat) {
    vec3 light_dir = light.position - fs_in.ws_position;
    float distance = length(light_dir);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    
    float diffuse = calculate_diffuse_component(light_dir, g_normal);
    float specular = calculate_specular_component(light_dir, g_normal, mat.shininess);

    vec3 ambient = light.ambient * mat.diffuse;
    vec3 diffuse_color = light.diffuse * diffuse * mat.diffuse;
    vec3 specular_color = light.specular * specular * mat.specular;

    return (ambient + diffuse_color + specular_color) * light.power * attenuation;
}

vec3 calculate_directional_light(DirectionalLight light, SampledMaterial mat) {
    float diffuse = calculate_diffuse_component(light.direction, g_normal);
    float specular = calculate_specular_component(light.direction, g_normal, mat.shininess);

    vec3 ambient = light.ambient * mat.diffuse;
    vec3 diffuse_color = light.diffuse * diffuse * mat.diffuse;
    vec3 specular_color = light.specular * specular * mat.specular;

    return (ambient + diffuse_color + specular_color) * light.power;
}

vec3 calculate_spot_light(SpotLight light, SampledMaterial mat) {
    float theta = dot(normalize(light.direction), normalize(fs_in.ws_position - light.position));

    if (theta > light.cut_off) {
        // Inside the cone
        vec3 light_dir = light.position - fs_in.ws_position;
        float distance = length(light_dir);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

        float diffuse = calculate_diffuse_component(light_dir, g_normal);
        float specular = calculate_specular_component(light_dir, g_normal, mat.shininess);

        vec3 ambient = light.ambient * mat.diffuse;
        vec3 diffuse_color = light.diffuse * diffuse * mat.diffuse;
        vec3 specular_color = light.specular * specular * mat.specular;

        return (ambient + diffuse_color + specular_color) * light.power * attenuation;
    } else {
        // Outside the cut off angle, use ambient light
        return light.ambient * light.power * mat.diffuse;
    }

}
