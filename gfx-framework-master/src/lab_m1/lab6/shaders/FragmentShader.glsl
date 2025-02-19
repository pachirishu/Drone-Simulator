#version 330

// Input from Vertex Shader
in vec3 frag_color;

// Output color
layout(location = 1) out vec4 out_color;

void main()
{
   
    out_color = vec4(frag_color, 1.0); 
}
