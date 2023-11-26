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
uniform int tile_count;

// Lighting
uniform float lambertian_coefficient;
uniform float specular_coefficient;
uniform float specular_power;
uniform float ambient_lighting;

// Material attributes (constants)
uniform vec3 object_color;

// Blinn-Phong shading
void main() 
{
    vec3 V, L, N, H;

    // Get substitute normal in tangent space from the normal map
    
    vec2 coord = tile_count * vertex_uv; // multiply by a constant (10) to tile the texture

    coord.y = 1.0 - coord.y;
    N = (texture2D(texture_map, coord).rgb);
    N = normalize(N * 2.0 - 1.0); // change scale from 0 to 1 --> -1 to 1
    
    V = normalize(view_vector);
    L = normalize(light_vector);
    H = (V + L);
    H = normalize(H);
    
    // AMBIENT
    float ambient = ambient_lighting;

    // DIFFUSE
    float lambertian = max(dot(N, L), 0.0);

    // SPECULAR
    float spec_angle = max(dot(N, H), 0.0);
    float specular = pow(spec_angle, specular_power); // NOTE: This one too (so lammbertian coefficient, specular coefficient, ambient, specular power)
    
    vec4 obj_col = vec4(object_color, 1.0);
    vec4 blue = vec4(0.5,0.5,1.0,0.45); // tinge everything blue!
    if (gl_FrontFacing){
        gl_FragColor = blue*(ambient + lambertian_coefficient*lambertian + specular_coefficient*specular)*obj_col; // NOTE: These three coefficients

    } else {
        gl_FragColor = 0.25*ambient*blue*obj_col;
    }
}
