#version 400

in vec3 position;
in vec2 uv;
uniform float timer;
uniform int hurt;
out vec2 vs_uv;

void main() {
    
    vec3 pos = position;
    gl_Position = vec4(pos, 1.0);
    vs_uv = uv;
}
