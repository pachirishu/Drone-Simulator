#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 color;

out vec3 fragColor; // Culoarea transmisa catre fragment shader
out vec3 fragNormal; // Vectorul normal pentru eventuale alte efecte
out vec3 fragPosition; // Pozitia in spatiul lumii

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(position, 1.0);

    // Transmitem datele catre fragment shader
    fragColor = color;
    fragNormal = mat3(transpose(inverse(model))) * normal;
    fragPosition = vec3(model * vec4(position, 1.0));
}
