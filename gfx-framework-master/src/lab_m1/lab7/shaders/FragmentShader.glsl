#version 330

// Intrări de la vertex shader
in vec3 frag_normal;
in vec3 frag_position;
in vec2 frag_texture_coord;

// Uniform variables
uniform sampler2D texture_1;
uniform vec3 light_position;
uniform vec3 eye_position;
uniform vec3 object_color;

uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;
uniform int has_texture;

// Ieșire
layout(location = 0) out vec4 out_color;

void main()
{
    vec3 color;
    
    // Dacă are textură, preia culoarea texturii
    if (has_texture == 1) {
        color = texture(texture_1, frag_texture_coord).rgb;
    } else {
        // Altfel, folosește culoarea obiectului
        color = object_color;
    }

    vec3 N = normalize(frag_normal);
    vec3 L = normalize(light_position - frag_position);
    vec3 V = normalize(eye_position - frag_position);
    vec3 H = normalize(L + V);

    // Componentă ambientală
    float ambient_light = 0.25;
    
    // Componentă difuză
    float diffuse_light = material_kd * max(dot(N, L), 0);

    // Componentă speculară
    float specular_light = 0;
    if (diffuse_light > 0)
    {
        specular_light = material_ks * pow(max(dot(N, H), 0), material_shininess);
    }

    // Combină componentele de iluminare cu culoarea
    float light = ambient_light + diffuse_light + specular_light;
    out_color = vec4(color * light, 1);
}