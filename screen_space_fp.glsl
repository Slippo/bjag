#version 130

// Passed from the vertex shader
in vec2 uv0;

// Passed from outside
uniform float timer;
uniform sampler2D texture_map;

float wave_strength = 0.001; // The strength of the water wobbliness. Too high is vomit-inducing...

void main() 
{
	// Screenspace effect
	vec2 pos = uv0;
    pos.x = pos.x + wave_strength*(sin(timer*8.2+8.0*pos.y));
	pos.y = pos.y;
	
	// Output
	vec4 colour = texture(texture_map, pos);
	vec4 pixel = vec4(colour.r - 0.1, colour.g - 0.1, colour.b + 0.15, colour.a) ;
	gl_FragColor = pixel;
}
