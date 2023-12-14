#version 140

// Attributes passed from the vertex shader
in vec3 vertex_position;
in vec2 uv_interp;
in vec4 color_interp;

// Uniform (global) buffer
uniform sampler2D texture_map; // Normal map

// Material attributes (constants)
uniform vec3 object_color;


void main() 
{
   // Retrieve texture value
	vec2 uv_use = 2*uv_interp;
    vec4 pixel = texture(texture_map, uv_use);

    // Use texture in determining fragment colour

    gl_FragColor = pixel;
}