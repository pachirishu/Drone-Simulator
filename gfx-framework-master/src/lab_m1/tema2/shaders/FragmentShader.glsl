#version 330 core

in vec3 FragPos;
in float Height;

out vec4 FragColor;

uniform vec3 color1;
uniform vec3 color2;

void main() {
    // Interpolam culorile pe baza inaltimii
    float t = clamp((Height + 0.5) / 1.0, 0.0, 1.0);
    vec3 finalColor = mix(color1, color2, t);

    FragColor = vec4(finalColor, 1.0);
}
