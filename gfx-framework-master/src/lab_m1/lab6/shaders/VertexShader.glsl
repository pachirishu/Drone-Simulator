#version 330

// Input attributes
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texCoord;
layout(location = 3) in vec3 v_color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform float Time; 


out vec3 frag_position;
out vec3 frag_normal;
out vec2 frag_texCoord;
out vec3 frag_color;

void main()
{
    vec3 animated_position = v_position;
    animated_position.y += sin(4.0 * Time) * 2.0;

 
    frag_color = v_color;
    frag_color.r *= abs(sin(20.0 * Time));


    frag_position = vec3(Model * vec4(animated_position, 1.0));
    frag_normal = mat3(transpose(inverse(Model))) * v_normal;
    frag_texCoord = v_texCoord;


    gl_Position = Projection * View * Model * vec4(animated_position, 1.0);
}
