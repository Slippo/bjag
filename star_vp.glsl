#version 400

// Vertex buffer
in vec3 vertex;
in vec3 normal;
in vec3 color;

// Uniform (global) buffer
uniform mat4 world_mat;
uniform mat4 view_mat;
uniform mat4 normal_mat;
uniform float timer;

// Attributes forwarded to the geometry shader
out vec3 vertex_color;
out float timestep;

// Simulation parameters (constants)
uniform vec3 up_vec = vec3(0.0, 1.0, 0.0);
uniform vec3 object_color = vec3(0.8, 0.8, 0.8);
float grav = 0.0; // Gravity
float speed = 2.0; // Controls the speed of the explosion
float upward = 0.0; // additional y velocity for all particles
const float pi = 3.14159265358979323;

// Linear congruential generator (LCG)
// Yields a float in the range [0..1]
float genRandom(float seed, int a, int b, int m){
    float r = fract(mod(seed * a + b, m));
    return r;
}
// Random Numbers
float rand_0 = genRandom(color.r,713597,376547,616027); // ID stored in color
float rand_1 = genRandom(color.r,984349,365257,408203);
float rand_2 = genRandom(color.r,266177,702913,611441);
// big primes
// 611441 493817 256129 365257 961663 408203 
// 525137 331883 765251 656809 984349 434191
// 702913 266177 271027 365257 961663 408203 

void main()
{

    float particle_id = color.r; // Derived from the particle color. We use the id to keep track of particles

    float phase = color.g;
    float theta = particle_id * pi;
    float phi = particle_id *4 *  pi;
    // Let time cycle every four seconds
    float circtime = timer - 1.0 * floor(timer / 1);
    float t = circtime + phase; // Our time parameter
    
    // Let's first work in model space (apply only world matrix)
    vec4 position = world_mat * vec4(vertex, 1.0);
    vec4 norm = normal_mat * vec4(normal, 0.0);

    if (t > 1.0)
    {
        t = t - 1;
    }

    // Make particles explode in a spherical shape
    /*
    position.x += t*sin(theta)*cos(phi);
    position.y += t*sin(theta)*sin(phi);
    position.z += t*(cos(theta));
    */
    position.x += sin(max(rand_0,max(rand_1,rand_2)) + speed*timer);
    position.z += cos(max(rand_0,max(rand_1,rand_2)) + speed*timer);
    position.y += cos(max(rand_0,max(rand_1,rand_2)) + speed*timer);
    // Now apply view transformation
    gl_Position = view_mat * position;
        
    // Define outputs
    // Define color of vertex
    //vertex_color = color.rgb; // Color defined during the construction of the particles
    //vertex_color = object_color; // Uniform color 
    //vertex_color = vec3(t, 0.0, 1-t); // red-purple dynamic color
    vertex_color = vec3(1.0, 1-t, 0.0); // red-yellow dynamic color

    // Forward time step to geometry shader
    timestep = t;
}
