#version 330 core

in vec3 fragColor;
out vec4 color;

// Uniforms
uniform vec3 color1; // Culoarea initiala a obiectului
uniform vec3 color2; // Alb-gri pentru interpolare
uniform float droneAltitude;
uniform float maxAltitude;

void main() {
    // Normalizare altitudine
    float factor = clamp(droneAltitude / maxAltitude, 0.0, 1.0);

    // Interpolare color1 si color2
    vec3 interpolatedColor = mix(color1, color2, factor);

    // Aplicarea culorii interpolate
    color = vec4(interpolatedColor, 1.0);
}
