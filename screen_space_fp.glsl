#version 400
in vec2 vs_uv;

uniform float timer;
uniform sampler2D texture_map;
//uniform sampler2D bubble;
uniform float oxygen;
uniform int hurt;

// Constants
float wave_strength = 0.001; // Sine wave strenght; vomit-inducing if > 0.001
vec4 colour = vec4(0,0,0,1);

// random number from 0 to 1 // e.g. float rand_t = genRandom(timer,713597,376547,616027);
float genRandom(float seed, int a, int b, int m){
    float r = fract(mod(seed * a + b, m));
    return r;
}

// Gives the viewer a "mask" which indicates if damage is being taken
void main() {

	vec4 outer_colour = vec4(0,0,0,1);
	outer_colour.r = mix(0.0,0.9, 1 - (oxygen * 1/480)); // make the edge flash red as time runs out
	outer_colour.r *= abs(sin(mix(1,3,hurt) * timer));

	vec2 pos = (vs_uv * 2) - 1; // - 1 to 1
	float distance_from_center = sqrt(pow(pos.x, 2) + pow(pos.y, 2));
	float radius = 0.9;
	hurt == 1 ? radius = mix(0.9,0.88, abs(sin(4*timer))):radius; // make view dilate when damaged

	// make the inner pixels wavy
	distance_from_center < radius ? pos.x = pos.x + 0.005*sin((15*hurt+2)*timer + 2*pos.y):pos.x; // waviness increases if damage is taken
	
	// make the inner pixels the scene pixels
	distance_from_center > radius ? colour = outer_colour:colour = texture(texture_map, 0.5*pos + 0.5);

	// blue tinge only if in the game world
	distance_from_center < radius ? colour = vec4(colour.r - 0.1, colour.g - 0.1, colour.b + 0.3, colour.a):colour = outer_colour;

	abs(distance_from_center - radius) < 0.03 ? colour += vec4(mix(0,1,hurt),0.3,0.7,0.2):colour; // outline the circle to resemble a "mask"
	// outline becomes red when damaged

	colour = 0.9*colour;
	gl_FragColor = colour;
}
