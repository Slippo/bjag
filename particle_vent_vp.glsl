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

// Pseudo-random number generator using particle_id
float rand(float id){
    vec2 r = vec2(id, -id);
    return fract(sin(dot(r, vec2(45.2918, 62.9918))) * 43758.5453);
}

void main()
{
    particle_id = color.r; // Derived from the particle color. We use the id to keep track of particles

    float t = timer - 10 * floor(timer / 10); // Time cycle (every 6 seconds)
    float phase = particle_id * pi;
    
    // Let's first work in model space (apply only world matrix)
    vec4 position = world_mat * vec4(vertex, 1.0);
    vec4 norm = normal_mat * vec4(normal, 0.0);

    // Particle "effect"
    position.x += dist * atan(t) * phase;
    position.y *= atan(t) + spread * phase;
    float random = rand(particle_id);
    position.y += t * t * (rise + 0.5*random);
    position.z += random;

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
