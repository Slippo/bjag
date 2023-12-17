#version 400
in vec2 vs_uv;

uniform float timer;
uniform sampler2D texture_map;
//uniform sampler2D bubble;
uniform float oxygen;

// Constants
float wave_strength = 0.001; // Sine wave strenght; vomit-inducing if > 0.001
vec4 colour = vec4(0,0,0,1);

// random number from 0 to 1 // e.g. float rand_t = genRandom(timer,713597,376547,616027);
float genRandom(float seed, int a, int b, int m){
    float r = fract(mod(seed * a + b, m));
    return r;
}

void main() 
{
	vec2 pos = (vs_uv * 2) - 1; // - 1 to 1
	//pos.x = pos.x + wave_strength*(sin(timer*4+8.0*pos.y));

	float distance_from_center = sqrt(pow(pos.x, 2) + pow(pos.y, 2));
	float radius = 0.9 * (oxygen * 1/oxygen);

	// inside circle? make it wavy
	distance_from_center < radius ? pos.x = pos.x + 0.005*sin(2*timer + 2*pos.y):pos.x;
	
	// outside circle ? make black. inside? render scene
	distance_from_center > radius ? colour = vec4(0,0,0,1):colour = texture(texture_map, 0.5*pos + 0.5);

	// blue tinge only if in the game world
	distance_from_center < radius ? colour = vec4(colour.r - 0.1, colour.g - 0.1, colour.b + 0.3, colour.a):colour = vec4(0,0,0,1);
	colour = 0.9*colour;
	gl_FragColor = colour;
}
