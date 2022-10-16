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
    vec3 position;      // 0x00
    vec3 ambient;       // 0x10
    vec3 diffuse;       // 0x20
    vec3 specular;      // 0x30
                        
    float power;        // 0x3C
    float constant;     // 0x40
    float linear;       // 0x44
    float quadratic;    // 0x48
};

struct DirectionalLight {
    vec3 direction;     // 0x00
    vec3 ambient;       // 0x10
    vec3 diffuse;       // 0x20
    vec3 specular;      // 0x30
    float power;        // 0x3C
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
#define MAX_POINT_LIGHTS 16
#define MAX_MATERIALS 16
#define MAX_TEXTURE_SLOTS 32

layout (std140) uniform LightParameters {
	PointLight u_PointLights[MAX_POINT_LIGHTS];
	DirectionalLight u_DirLight;
//	SpotLight u_SpotLight;
};

// Uniforms
uniform vec3 u_ViewPos;
uniform SpotLight u_SpotLight;
uniform float u_Time;
uniform samplerCube u_skybox;
uniform Material u_Materials[MAX_MATERIALS];
//uniform sampler2D u_Textures[MAX_TEXTURE_SLOTS];

// Vertex shader inputs
in VS_OUT {
    vec2 TexCoords;
    vec3 Position;
    vec3 Normal;
	float MaterialIndex;
} fs_in;

out vec4 g_Color;

vec3 g_Normal = vec3(0.0);

vec3 calculate_point_light(PointLight light, SampledMaterial mat);
vec3 calculate_directional_light(DirectionalLight light, SampledMaterial mat);
vec3 calculate_spot_light(SpotLight light, SampledMaterial mat);

void main() {
    g_Normal = normalize(fs_in.Normal);
	int material_index = int(fs_in.MaterialIndex);
    
    vec3 mat_ambient_diffuse = texture(u_Materials[material_index].diffuse, fs_in.TexCoords).rgb;
    vec3 mat_specular = texture(u_Materials[material_index].specular, fs_in.TexCoords).rgb;
    SampledMaterial mat = SampledMaterial(mat_ambient_diffuse, mat_specular, u_Materials[material_index].shininess);

    vec3 final_color = vec3(0.0);

    final_color += calculate_directional_light(u_DirLight, mat);
    for (int i = 0; i < MAX_POINT_LIGHTS; i++) {
        final_color += calculate_point_light(u_PointLights[i], mat);
    }

#if 0
    if (gl_FragCoord.x < 960) {
        const float ratio = 1.0 / 1.52;
        vec3 V = normalize(fs_in.Position - u_view_pos);
        vec3 R = refract(V, g_Normal, ratio);

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
    vec3 V = normalize(u_ViewPos - fs_in.Position);
    return pow(max(dot(R, V), 0.0), shininess);
}

vec3 calculate_point_light(PointLight light, SampledMaterial mat) {
    vec3 light_dir = light.position.xyz - fs_in.Position;
    float distance = length(light_dir);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    
    float diffuse = calculate_diffuse_component(light_dir, g_Normal);
    float specular = calculate_specular_component(light_dir, g_Normal, mat.shininess);

    vec3 ambient = light.ambient.xyz * mat.diffuse;
    vec3 diffuse_color = light.diffuse.xyz * diffuse * mat.diffuse;
    vec3 specular_color = light.specular.xyz * specular * mat.specular;

    return (ambient + diffuse_color + specular_color) * light.power * attenuation;
}

vec3 calculate_directional_light(DirectionalLight light, SampledMaterial mat) {
    float diffuse = calculate_diffuse_component(light.direction.xyz, g_Normal);
    float specular = calculate_specular_component(light.direction.xyz, g_Normal, mat.shininess);

    vec3 ambient = light.ambient.xyz * mat.diffuse;
    vec3 diffuse_color = light.diffuse.xyz * diffuse * mat.diffuse;
    vec3 specular_color = light.specular.xyz * specular * mat.specular;

    return (ambient + diffuse_color + specular_color) * light.power;
}

vec3 calculate_spot_light(SpotLight light, SampledMaterial mat) {
    float theta = dot(normalize(light.direction), normalize(fs_in.Position - light.position));

    if (theta > light.cut_off) {
        // Inside the cone
        vec3 light_dir = light.position - fs_in.Position;
        float distance = length(light_dir);
        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

        float diffuse = calculate_diffuse_component(light_dir, g_Normal);
        float specular = calculate_specular_component(light_dir, g_Normal, mat.shininess);

        vec3 ambient = light.ambient * mat.diffuse;
        vec3 diffuse_color = light.diffuse * diffuse * mat.diffuse;
        vec3 specular_color = light.specular * specular * mat.specular;

        return (ambient + diffuse_color + specular_color) * light.power * attenuation;
    } else {
        // Outside the cut off angle, use ambient light
        return light.ambient * light.power * mat.diffuse;
    }

}
