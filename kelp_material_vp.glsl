// Material with no illumination simulation
#version 130

// Vertex buffer
in vec3 vertex;
in vec3 color;

// Uniform (global) buffer
uniform mat4 world_mat;
uniform mat4 view_mat;
uniform mat4 projection_mat;
uniform float timer;
uniform int collision;
uniform int node_type;

// Attributes forwarded to the fragment shader
out vec4 color_interp;

void main()
{
    gl_Position = projection_mat * view_mat * world_mat * vec4(vertex, 1.0);
    //color_interp = vec4(0.1 + cos(length(vertex)),0.5 + sin(length(vertex)),0,1); // orange stripes
    //color_interp = vec4(0.1,0.5 + sin(length(vertex)),0,1); // green stripes
    
    vec4 base_color = vec4(0.5,0.5 + 0.25 * sin(length(vertex)),0.5,1); //blue/grey/green

    color_interp = base_color;
    if(node_type == 1){
        color_interp *= 0.5;
        if(vertex.y > 0){
            color_interp = 0.75 * base_color;
        }
        if(length(vec2(vertex.x,vertex.z)) < 0.5 && vertex.y > 0){
            color_interp = 1.25 * base_color;
        }
        if(length(vec2(vertex.x,vertex.z)) < 0.25 && vertex.y > 0){
            color_interp *= vec4(0.31, 0.30, 0.53, 1.0);
        }
    }
}
