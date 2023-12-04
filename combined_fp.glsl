#version 140

// Attributes passed from the vertex shader
in vec3 vertex_position;
in vec2 vertex_uv;

in vec3 light_vector;
in vec3 view_vector;
in vec3 normal_vector;
in vec3 tangent_frag_pos;
in vec3 tangent_light_pos;
in vec3 tangent_view_pos;

// Uniform (global) buffer
uniform sampler2D texture_map; // Normal map

// Material attributes (constants)
uniform vec3 object_color;

// Blinn-Phong shading
void main() 
{
    vec3 V, L, N, H;

   

    // Get substitute normal in tangent space from the normal map
    
    vec2 coord = 10 * vertex_uv; // multiply by a constant (10) to tile the texture
    coord.y = 1.0 - coord.y;
    N = normalize(normal_vector);
    //N = normalize(N * 2.0 - 1.0); // change scale from 0 to 1 --> -1 to 1
    
    V = normalize(view_vector);
    L = normalize(light_vector);
    H = (V + L);
    H = normalize(H);
    
    // AMBIENT
    float ambient = 0.4;

    // DIFFUSE
    float lambertian = max(dot(N, L), 0.0);

    // SPECULAR
    float spec_angle = max(dot(N, H), 0.0);
    float specular = pow(spec_angle, 278.0);
    
    //vec4 obj_col = vec4(object_color, 1.0);

     // Retrieve texture value
	vec2 uv_use = vertex_uv;
    vec4 pixel = texture(texture_map, uv_use);

     //gl_FragColor = lightcol*pixel*diffuse + lightcol*vec4(1,1,1,1)*spec + // specular might not be colored lightcol*pixel*amb; // ambcol not used, could be included here

    if (gl_FrontFacing){
        gl_FragColor = pixel*lambertian + pixel*specular + pixel*ambient;
    } else {
        gl_FragColor = pixel*0.25*ambient;
    }

    if(gl_FragColor.a < 0.1) discard;
}