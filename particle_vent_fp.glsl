#version 400

// Attributes passed from the geometry shader
in vec4 frag_color;

// Uniform (global) buffer
uniform sampler2D texture_map; // Texture map
in vec2 texture_coord; // Texture coordinates

void main (void)
{
    // Very simple fragment shader, but we can do anything we want here
    // We could apply a texture to the particle, illumination, etc.

    //gl_FragColor = frag_color;

    

    gl_FragColor = texture(texture_map, texture_coord); // Apply texture to fragment

    if (gl_FragColor.a < 0.1) discard;
    
}
