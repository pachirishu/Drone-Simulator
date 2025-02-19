#version 330


in vec3 world_position;
in vec3 world_normal;


uniform vec3 light_position[2];
uniform vec3 light_direction[2];
uniform vec3 eye_position;

uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

uniform vec3 object_color;
uniform float cutOffAngle;
uniform int useSpotlight;

layout(location = 0) out vec4 out_color;

void main()
{
    vec3 normal = normalize(world_normal);

    vec3 V = normalize(eye_position - world_position);

    vec3 totalLighting = vec3(0.0);

    for (int i = 0; i < 2; i++) {
        vec3 L = normalize(light_position[i] - world_position);

        vec3 R = reflect(-L, normal);

        float ambient_strength = 0.25;
        vec3 ambient = ambient_strength * object_color;

        float diffuse_strength = max(dot(normal, L), 0.0);
        vec3 diffuse = material_kd * diffuse_strength * object_color;

        float specular_strength = pow(max(dot(V, R), 0.0), material_shininess);
        vec3 specular = material_ks * specular_strength * vec3(1.0);

        float d = 0.5f;
        vec3 lighting = ambient + (diffuse + specular) * (d*d);

        if (useSpotlight == 1) {
            // float theta = dot(-L, normalize(light_direction[i]));
            // float epsilon = 0.1; // Margine pentru tranziție lină
            // float intensity = smoothstep(cos(cutOffAngle + epsilon), cos(cutOffAngle), theta);
            float cut_off = radians(30.0f);
            float spot_light = dot(-L, light_direction[i]);
            float spot_light_limit = cos(cut_off);
            if (spot_light > spot_light_limit) {
            float linear_att = (spot_light - spot_light_limit) / (1.0f - spot_light_limit);
            float linear_attenuation = pow(linear_att, 2);

            lighting = ambient + linear_attenuation * (diffuse + specular) * (d*d);}
            else lighting = ambient;
        }

        totalLighting += lighting;
    }
    float ambient_strength = 0.25;
    totalLighting -= ambient_strength * object_color;


    totalLighting = clamp(totalLighting, 0.0, 1.0);

    out_color = vec4(totalLighting, 1.0);
}
