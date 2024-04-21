#shader vertex
#version 460 compatibility

// Built for 4xPos, 2xUV, 3xNorm vertices
in vec4 in_position;
in vec2 in_uv;
in vec3 in_normal;

// Only using UV
out vec4 pos;
out vec2 uv;

uniform mat4 screenspace, transform;

void main() {
	gl_Position = screenspace * transform * in_position;
	pos = in_position;
	uv = in_uv;
}

#shader fragment
#version 460 compatibility

in vec4 pos;
in vec2 uv;

// Uniform inputs, determining whether the shader will use the given texture or not
uniform vec4 color = vec4(1);
uniform sampler2D texture;
uniform bool textured = true;

out vec4 out_color;

void main() {
	if(textured)
		out_color = texture2D(texture, uv);
	else
		out_color = color;
}