#version 400

in vec3 position;
in vec2 uv;

out vec2 vs_uv;

void main()
{
    gl_Position = vec4(position, 1.0);

    vs_uv = uv;
}
