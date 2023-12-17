// Where the magic happens
// All transformations are applied
// on a per-invocation basis
#version 400
#define PI 3.1415926538
// Definition of the geometry shader
layout (triangles) in;
layout (invocations = 8) in; // we want 24 fish
layout (triangle_strip, max_vertices = 3) out;

// Attributes passed from the vertex shader
in vec3 vertex_color[];
in float timestep[];
in vec2 uv_interp[];
in vec3 normal_interp[];
in vec3 light_pos_vp[];
// Uniform (global) buffer
uniform mat4 projection_mat;
uniform mat4 view_mat;
uniform mat4 normal_mat;

// Attributes passed to the fragment shader
out vec4 frag_color;
out vec2 tex_coord;
out float invocationID;
out float num_invocations;
out vec3 position_interp;
out vec3 normal_interp_g;
out vec3 light_pos_out;

// Linear congruential generator (LCG)
// Yields a float in the range [0..1]
float genRandom(float seed, int a, int b, int m){
    float r = fract(mod(seed * a + b, m));
    return r;
}
// Random Numbers
// The literals are 6-digit prime numbers, picked randomly from the internet
// The seed should differ based on the invocation we are currently working on
float rand_0 = genRandom(gl_InvocationID,713597,376547,616027);
float rand_1 = genRandom(vertex_color[0].r, 611441,493817,256129);
float rand_2 = genRandom(gl_PrimitiveIDIn, 525137,331883,765251);
float rand_3 = genRandom(gl_InvocationID,702913,266177,271027);
float rand_4 = genRandom(rand_3, 365257,961663,408203);
float rand_5 = genRandom(uv_interp[0].x + 7/uv_interp[0].y, 656809,984349,434191);

// Convert cartesian coordinates to cylindrical coordinates https://en.wikipedia.org/wiki/Cylindrical_coordinate_system
// in this version, the input vector is in the form:
// radius, height, angle (y-axis)
vec3 CylindricalToCartesian(vec3 in_pos){
    vec3 out_pos;
    out_pos.x = in_pos.x * cos(in_pos.z);
    out_pos.y = in_pos.y;
    out_pos.z = in_pos.x * sin(in_pos.z);
    return out_pos;
}

// Rotational matrices for any angle
mat4 rotateX(float angle) {
	return mat4(	1.0,		0,			0,			0,
			 		0, 	cos(angle),	-sin(angle),		0,
					0, 	sin(angle),	 cos(angle),		0,
					0, 			0,			  0, 		1);
}
mat4 rotateY(float angle){
	return mat4(	cos(angle),		0,		sin(angle),	0,
			 				0,		1.0,			 0,	0,
					-sin(angle),	0,		cos(angle),	0,
							0, 		0,				0,	1);
}
mat4 rotateZ( in float angle ) {
	return mat4(	cos(angle),		-sin(angle),	0,	0,
			 		sin(angle),		cos(angle),		0,	0,
							0,				0,		1,	0,
							0,				0,		0,	1);
}

// Transformation matrix to move the fish along a ring-like path
mat4 fish(int id, float t, int iter, float variant){
    // Only vary the radius and height, rotation is handled separately

    vec3 cylindrical_coord = vec3(
        mix(0.3,.95,sin(id + t/7)*0.5 +0.5) + (cos(0.5*(id)) * 0.5 + 1), //radius 0.8 to 0.85
        mix(0.25,0.75,cos(id+variant*id)*0.5 +0.5) + 0.25*(cos(id*variant*PI)), // height 0.25 to 0.75
        0+ variant*variant // angle
    );
        
    vec3 coord = CylindricalToCartesian(cylindrical_coord);

    // Translation matrix
    // Accounts for translation to the cylindrical coordinate
    mat4 transf = mat4(
    vec4(1.0,0.0,0.0, 0.0),
    vec4(0.0, 1.0, 0.0, 0.0),
    vec4(0.0, 0.0, 1.0, 0.0),
    vec4(coord, 1.0)
    );

    //mat4 rota = mat4(1);
    //rota = rotateY(PI/2);
    mat4 rota = rotateZ(0.01 * sin(id*variant + 5*t))
    * rotateX(0.2*sin((t) + 0.2*id*t))
    * rotateY(mix(0,2,mod((mix(0,1, sin(id*variant + 1-t)) + id*id + 3*t)/10, 1)) *  PI); // rotation about the Y axis

    transf = rota * transf;
    return transf;
}

void main(void){
    float rand_j = gl_InvocationID + timestep[0];
    mat4 rotX = rotateX(0);
    mat4 rotY = rotateY(0);
    mat4 rotZ = rotateZ(0);
    num_invocations = 24;
    float variant = rand_0 - 0.5; // random number from -0.5 to 0.5
    for(int j = 0; j < 3; j++){
        rotY = rotateY((1+gl_InvocationID)*-timestep[0] * PI/2);
        rand_j = gl_InvocationID + timestep[j];
        mat4 rotation_mat = rotY;

        vec4 pos = gl_in[j].gl_Position;
        
        mat4 fish_trans = fish(gl_InvocationID, timestep[j], j, variant);
        
        gl_PrimitiveID = gl_PrimitiveIDIn;
        gl_Position = projection_mat * view_mat * fish_trans * pos;
        //gl_Position /= gl_Position.w;
        position_interp = vec3(view_mat * fish_trans * pos);
        //gl_Position += projection_mat * view_mat * fish(gl_InvocationID, timestep[j]);
        
        frag_color = vec4(vertex_color[j], 1.0);
        tex_coord = uv_interp[j];

        //vec3((normal_mat * fish_trans) * view_mat * vec4(light_pos[j],1));// * light_pos[0]);
        normal_interp_g = vec3((normal_mat * view_mat * fish_trans * vec4(normal_interp[j],0)));
        light_pos_out = vec3( view_mat * vec4(light_pos_vp[j],1));
        invocationID = gl_InvocationID;
        EmitVertex();
    }
    
    
    EndPrimitive();
}
 