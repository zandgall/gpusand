#shader vertex
#version 460 compatibility

// Built for 4xPos, 2xUV, 3xNorm vertices
in vec4 in_position;
in vec2 in_uv;
in vec3 in_normal;

// Only using UV
//out vec4 pos;
out vec2 uv;
out vec2 size;

uniform mat4 screenspace, transform;

void main() {
	gl_Position = screenspace * transform * in_position;
	//pos = in_position;

	size = (transform * vec4(1) - transform * vec4(-1, -1, -1, 1)).xy;

	uv = in_uv;
	//uv.x *= size.x;
	//uv.y *= size.y;
}

#shader fragment
#version 460 compatibility

//in vec4 pos;
in vec2 uv;
in vec2 size;

// Uniform inputs, determining whether the shader will use the given texture or not
uniform sampler2D texture;

uniform float pixel_scale = 1;

uniform vec4 line;
uniform vec4 dark;
uniform vec4 mid;
uniform vec4 light;

out vec4 out_color;

void main() {
	vec2 nuv = uv;
	ivec2 tex_size = textureSize(texture, 0);
	if(uv.x <= (pixel_scale*tex_size.x)/(2*size.x) && uv.x <= 0.5)
		nuv.x = size.x*uv.x/(pixel_scale*tex_size.x);//pixel_scale;
	else if (uv.x >= 1 - (pixel_scale*tex_size.x)/(2*size.x) && uv.x >= 0.5)
		nuv.x = size.x*(uv.x - 1)/(pixel_scale*tex_size.x)+1;
	else
		nuv.x = 0.5;

	if(uv.y <= (pixel_scale*tex_size.y)/(2*size.y) && uv.y <= 0.5)
		nuv.y = size.y*uv.y/(pixel_scale*tex_size.y);//pixel_scale;
	else if (uv.y >= 1 - (pixel_scale*tex_size.y)/(2*size.y) && uv.y >= 0.5)
		nuv.y = size.y*(uv.y - 1)/(pixel_scale*tex_size.y)+1;
	else
		nuv.y = 0.5;

	out_color = texture2D(texture, nuv);
	if(out_color.w==0)
		discard;
	if(out_color.x == 1 && out_color.y == 1 && out_color.z == 1)
		out_color = line;
	else
		out_color = out_color.x * light + out_color.y * mid + out_color.z * dark;
	//out_color.y = uv.x;
	//out_color.x = nuv.x;
	//out_color.z = nuv.z;
	//out_color.w = 1;
	//out_color.x = uv.x / pixel_scale * tex_size.x * 0.01;
	//out_color.y = min_size * 0.01;
	//out_color.w = 1;
}