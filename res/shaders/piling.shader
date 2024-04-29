#version 460 compatibility

#define X_SIZE 16
#define Y_SIZE 16

layout (local_size_x = X_SIZE, local_size_y = Y_SIZE, local_size_z = 1) in;

shared uint world_dead[64*64];
layout (rgba32f, binding = 1) uniform image2D world_color;
//layout (r32ui, binding = 1) uniform uimage2D world_dead;

layout (location = 2) uniform int frame;
layout (rgba32f, location = 3) uniform readonly image1D noise;

bool match(uint a, readonly uimage1D img) {
	for(int i = 0, n = imageSize(img); i < n; i++)
		if(imageLoad(img, i).r == a)
			return true;
	return false;
}

layout (r32ui, location = 4) uniform readonly uimage1D fallable;
layout (r32ui, location = 5) uniform readonly uimage1D solid;
layout (r32ui, location = 6) uniform readonly uimage1D passthrough;

void main() {
	ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
	ivec2 bounds = imageSize(world_color), c = coord;
	int xmir = imageLoad(noise, frame).r < 0.5 ? 1 : -1;
	c = coord;
	uint a = world_dead[c.x+c.y*64];
	if(c.x >= bounds.x || c.y >= bounds.y || !match(a, fallable))
		return;

	c = coord + ivec2(0,1);
	uint b = world_dead[c.x+c.y*64];
	if(c.x >= bounds.x || c.y >= bounds.y || !match(b, solid))
		return;

	c = coord + ivec2(xmir * 1, 1);
	uint _c = world_dead[c.x+c.y*64];
	if(c.x >= bounds.x || c.y >= bounds.y || !match(_c, passthrough))
		return;

	world_dead[coord.x + coord.y*64] = _c;
	world_dead[(coord.x+xmir) + (coord.y+1)*64] = a;

	vec4 d = imageLoad(world_color, coord);
	vec4 e = imageLoad(world_color, coord+ivec2(xmir, 1));
	vec4 f = imageLoad(world_color, coord+ivec2(0,1));

	imageStore(world_color, coord, e);
	imageStore(world_color, coord+ivec2(xmir,1), d);
}