#version 400

// Attributes passed from the geometry shader
in vec4 frag_color;
uniform sampler2D texture_map;
in vec2 tex_coord;

void main (void)
{
    
    //set fragment colour to texture at specific texture coordinates
    gl_FragColor = texture(texture_map, tex_coord);

    //if alpha value less thant 0.6, do not overrite fragment, this is to help with transparency occlusion issues "blending"
    if(gl_FragColor.a < 0.1) discard;

}
