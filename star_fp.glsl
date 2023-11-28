#version 400

// Attributes passed from the geometry shader
in vec4 frag_color;
in vec2 uv;
uniform sampler2D texture_map;
void main (void)
{
    // Very simple fragment shader, but we can do anything we want here
    // We could apply a texture to the particle, illumination, etc.
    vec4 coord = texture(texture_map, uv);

    gl_FragColor = coord;

    if(gl_FragColor.a < 0.61) discard;
}
