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
out float particle_id;

// Simulation parameters (constants)
uniform vec3 up_vec = vec3(0.0, 1.0, 0.0);
uniform vec3 object_color = vec3(0.8, 0.8, 0.8);
float spread = 3.0; // Controls the spread as particles travel further
float dist = 1.5; // Distance of vent stream
float bubbliness = 9.0; // Volatility of particles
float rise = 0.1; // The speed at which the particles ascend

// Useful constants
const float pi = 3.1415926536;
const float pi_over_two = 1.5707963268;
const float two_pi = 2.0*pi;

// Pseudo-random number generator
float rand(float id){
    return fract(sin(dot(vec2(id, -id), vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
    particle_id = color.r; // Derived from the particle color. We use the id to keep track of particles

    float t = timer - 8 * floor(timer / 8); // Time cycle (every 8 seconds)
    float phase = (particle_id + 2) * pi;

    // Let's first work in model space (apply only world matrix)
    vec4 position = world_mat * vec4(vertex, 1.0);
    vec4 norm = normal_mat * vec4(normal, 0.0);

    // Particle "effect"
    float random = rand(particle_id) / 100;
    position.x += random;
    position.y += (t * phase) * 0.03;
    position.z += random;

    // Now apply view transformation
    gl_Position = view_mat * position;
        
    // Define outputs
    vertex_color = vec3(1.0, 1-t, 0.0); // red-yellow dynamic color

    // Forward time step to geometry shader
    timestep = t;
}
