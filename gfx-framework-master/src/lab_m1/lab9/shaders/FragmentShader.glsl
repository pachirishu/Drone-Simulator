#version 330

// Input
in vec2 texcoord;

// Uniform properties
uniform sampler2D texture_1;
uniform sampler2D texture_2;
// TODO(student): Declare various other uniforms
uniform int mode;

// Output
layout(location = 0) out vec4 out_color;


void main()
{
    vec4 color_1 = texture2D(texture_1, texcoord);
    vec4 color_2 = texture2D(texture_2, texcoord);
    vec4 combined_color;
   if (mode == 0) {
	   combined_color = color_1;
   }
   if (mode == 1) {
   float mixFactor = 0.5;
   combined_color = mix(color_1, color_2, mixFactor);
   }

   if (combined_color.a < .5f) {
	   discard;
       }
    // TODO(student): Calculate the out_color using the texture2D() function.
    out_color = combined_color;

}
