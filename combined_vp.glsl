#version 140

// Vertex buffer
in vec3 vertex;
in vec3 normal;
in vec3 color;
in vec2 uv;

// Uniform (global) buffer
uniform mat4 world_mat;
uniform mat4 view_mat;
uniform mat4 projection_mat;
uniform mat4 normal_mat;
uniform vec3 view_pos;
uniform vec3 light_pos;

// Attributes forwarded to the fragment shader
out vec3 vertex_position;
out vec2 vertex_uv;

out vec3 tangent_light_pos;
out vec3 tangent_frag_pos;
out vec3 tangent_view_pos;

out vec3 light_vector;
out vec3 view_vector;
out vec3 normal_vector;

void main()
{
    
    gl_Position = projection_mat * view_mat * world_mat * vec4(vertex, 1.0);

    vertex_position = vec3(view_mat * world_mat * vec4(vertex, 1.0));

    // Define vertex tangent, bitangent and normal (TBN)
    vec3 vertex_normal = normalize(vec3(normal_mat * view_mat * vec4(normal, 0.0)));
    vec3 tangent = (color*2) -1; // We stored the tangent in the vertex color
    vec3 vertex_tangent_ts = normalize(vec3(normal_mat * view_mat * vec4(tangent, 0.0)));
    vec3 vertex_bitangent_ts = cross(vertex_normal, vertex_tangent_ts);

    // TBN matrix allows transition from view space to tangent space
    mat3 TBN_mat = transpose(mat3(vertex_tangent_ts, vertex_bitangent_ts, vertex_normal));

    // view-space positions
    vec3 light_pos_v = vec3(view_mat * vec4(light_pos,1.0));
    vec3 view_pos_v = vec3(view_mat * vec4(view_pos,1.0));
    
    // tangent-space positions
    tangent_light_pos = light_pos_v;
    tangent_view_pos = view_pos_v;
    tangent_frag_pos = vertex_position;
    normal_vector = vertex_normal;
    light_vector = tangent_frag_pos;
    view_vector = tangent_frag_pos;

    // Send texture coordinates
    vertex_uv = uv;




}