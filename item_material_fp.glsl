// Material with no illumination simulation

#version 130

// Attributes passed from the vertex shader
in vec4 color_interp;
in vec4 vertex_interp;
in vec2 uv_interp;
uniform sampler2D texture_map;


void main() {
	vec4 tex = texture(texture_map, uv_interp);
	gl_FragColor = tex;
	//gl_FragColor = vec4(0.6, 0.6, 0.6, 1.0);
}
