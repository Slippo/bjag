// Mesh Particle Shader (vertex -> geometry -> fragment)
// Allows a 3D mesh to be instanced multiple times for
// the purposes of a particle system
#version 400
#define PI 3.1415926538
// Vertex buffer
in vec3 vertex;
in vec3 normal;
in vec3 color;
in vec2 uv;
// Uniform (global) buffer
uniform mat4 world_mat;
uniform mat4 view_mat;
uniform float timer;
uniform vec3 light_pos;
// Attributes forwarded to the geometry shader
out vec3 vertex_color;
out float timestep;
out vec2 uv_interp;
out vec3 normal_interp;
out vec3 light_pos_vp;

// Linear congruential generator (LCG)
// Yields a float in the range [0..1]
float genRandom(float seed, int a, int b, int m){
    float r = fract(mod(seed * a + b, m));
    return r;
}
// Random Numbers
float rand_0 = genRandom(timer,713597,376547,616027);
//float rand_1 = genRandom(rand_0, 611441,493817,256129);
//float rand_2 = genRandom(gl_PrimitiveIDIn, 525137,331883,765251);
//float rand_3 = genRandom(gl_InvocationID,702913,266177,271027);
//float rand_4 = genRandom(rand_3, 365257,961663,408203);
//float rand_5 = genRandom(uv_interp[0].x + 7/uv_interp[0].y, 656809,984349,434191);

void main()
{
    // Pass the data from the mesh to the geometry shader
    // We only want to go with world-space coordinates for now
    light_pos_vp = vec3(vec4(light_pos,1)); // already in world space
    vec4 position = world_mat * 10* vec4(vertex, 1.0);

    vec4 norm = normalize(vec4(normal, 0.0)); // dont apply any matrices yet, all done in the geometry shader
    normal_interp = vec3(norm);
    gl_Position = position;
    uv_interp = uv;
    vertex_color = color;
    vertex_color.r = rand_0;
    
    timestep = timer;
}
