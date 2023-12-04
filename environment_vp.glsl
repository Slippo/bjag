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
uniform vec3 up_vec = vec3(0.0, 5.0, 0.0);
uniform vec3 object_color = vec3(0.8, 0.8, 0.8);
float grav = 0.0; // Gravity
float speed = 3.0; // Controls the speed of the explosion
float upward = 0.1; // additional y velocity for all particles
float spread = 5;


// Define some useful constants
const float pi = 3.1415926536;
const float pi_over_two = 1.5707963268;
const float two_pi = 2.0*pi;

void main()
{
    // Let time cycle every six seconds
    float circtime = timer - 6.0 * floor(timer / 6);
    float t = circtime; // Our time parameter
    
    // Let's first work in model space (apply only world matrix)
    vec4 position = world_mat * vec4(vertex, 1.0);
    vec4 norm = normal_mat * vec4(normal, 0.0);

    float particle_id = color.r; // Derived from the particle color. We use the id to keep track of particles

    float phase = (two_pi*particle_id); //phase is created so that particals can be drawn out of sync

    // position in the x direction is exponential, to give us a rapid explosion of force that peters overtime, with respect to the
    //normal of each particle
    position.x += ((norm.x*t) * (norm.x*t)) * spread; 
    
	
	// movement in the y direction is also exponential, gravity is added to slow the extreme vertical movement overtime
    //factor in phase so particals move up at different speeds
    position.y += speed * ((phase*t - grav*up_vec.y*t)) + upward * t * t * t * t;
    
    position.z += ((norm.z*t) * (norm.z*t)) * spread; 
    
    
    // Now apply view transformation
    gl_Position = view_mat * position;
        
    // Define outputs
    // Define color of vertex
    
    vertex_color = vec3(1.0, 1-t, 0.0); // red-yellow dynamic color

    // Forward time step to geometry shader
    timestep = t;
}