#version 400
#define PI 3.1415926538
// Attributes passed from the geometry shader
in vec4 frag_color;
in vec2 tex_coord;
in float invocationID;
in float num_invocations;
in vec3 light_pos_out;

in vec3 position_interp;
in vec3 normal_interp_g;
in vec3 normal_interp;
in vec3 light_pos;

out vec4 out_color;

uniform sampler2D texture_map;
uniform float rand_u;

// Material attributes (constants)
vec4 ambient_color = vec4(0.6, 0.6, 0.6, 1.0);
vec4 diffuse_color = vec4(0.8, 0.8, 0.8, 1.0);
vec4 specular_color = vec4(1,1,1,1);
float phong_exponent = 128.0;
float Ia = 0.45; // Ambient light amount

void main (void)
{
     vec3 N, // Interpolated normal for fragment
         L, // Light-source direction
         V, // View direction
         H; // Half-way vector

    // Compute Lambertian term Id
    N = normalize(normal_interp_g);

        // Compute specular term Is for Blinn-Phong shading
    V = - position_interp; // Eye position is (0, 0, 0) in view coordinates
    V = normalize(V);

    // fix problem with the normals on the fins of the fish
    if(dot(V,N) < 0){
        N = -N;
    }

    L = (light_pos_out - position_interp);
    L = normalize(L);

    float Id = max(dot(N, L), 0.0);

    H = (V + L); // Halfway vector (will be normalized anyway)
    H = normalize(H);

    float spec_angle_cos = max(dot(N, H), 0.0);
    float Is = pow(spec_angle_cos, phong_exponent);

    // Vary the color per-object
    vec4 special_color = vec4(
    mix(0.7,0.95, float(invocationID)/float(num_invocations)),
    mix(0.7,0.89, rand_u * float(invocationID)/float(num_invocations)),
    mix(0.85,0.71, float(invocationID)/float(num_invocations)),
    1);

    out_color = (Ia*ambient_color + Id*diffuse_color + Is*specular_color) *
    special_color * texture(texture_map, vec2(tex_coord.x, 1-tex_coord.y));
    //out_color = out_color/out_color.w;
    out_color.w = 1;
}