// Material with no illumination simulation
#version 130

// Vertex buffer
in vec3 vertex;
in vec3 color;
in vec2 uv;

// Uniform (global) buffer
uniform mat4 world_mat;
uniform mat4 view_mat;
uniform mat4 projection_mat;
uniform float timer;
uniform int collision;
uniform int node_type;

// Attributes forwarded to the fragment shader
out vec4 color_interp;
out vec4 vertex_interp;
out vec2 uv_interp;

void main()
{
    gl_Position = projection_mat * view_mat * world_mat * vec4(vertex, 1.0);
    
    vec4 base_color = vec4(0.5,0.5 + 0.25 * sin(length(vertex)),0.5,1); //blue/grey/green
    vertex_interp = view_mat * world_mat * vec4(vertex, 1.0);
    color_interp = base_color;
    uv_interp = uv;
}
