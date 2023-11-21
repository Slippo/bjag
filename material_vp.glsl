// Material with no illumination simulation

#version 130

// Vertex buffer
in vec3 vertex;
in vec3 color;

// Uniform (global) buffer
uniform mat4 world_mat;
uniform mat4 view_mat;
uniform mat4 projection_mat;
uniform int collision;

// Attributes forwarded to the fragment shader
out vec4 color_interp;

//uniforms for custom object colors
uniform int node_type;
uniform vec3 object_color;


void main()
{
    gl_Position = projection_mat * view_mat * world_mat * vec4(vertex, 1.0);

    color_interp = vec4(colour_type, 1.0);

    //if(node_type == 1){
       // color_interp = vec4(colour_type, 1.0);
   // }else{
       // color_interp = vec4(color, 1.0);
   // }
}
