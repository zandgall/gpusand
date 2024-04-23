#version 460 compatibility

layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

layout (rgba32f, binding = 0) uniform image2D world_color;
layout (r32ui, binding = 1) uniform uimage2D world_dead;

layout (location = 0) uniform int frame;
layout (rgba32f, location = 1) uniform readonly image1D noise;
layout (r32ui, location = 2) uniform readonly uimage1D gravity_affected;
layout (r32ui, location = 3) uniform readonly uimage1D non_solid;
layout (r32ui, location = 4) uniform readonly uimage1D solid;

bool match(uint a, readonly uimage1D img) {
	for(int i = 0, n = imageSize(img); i < n; i++)
		if(imageLoad(img, i).r == a)
			return true;
	return false;
}

void main() {
	ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
	int xmir = imageLoad(noise, frame).r < 0.5 ? 1 : -1;
	uint a = imageLoad(world_dead, coord).r, b = imageLoad(world_dead, coord+ivec2(0,1)).r, c = imageLoad(world_dead, coord+ivec2(xmir,1)).r;
	if(match(a, gravity_affected) && match(b, solid) && match(c, non_solid)) {
		vec4 d = imageLoad(world_color, coord), e = imageLoad(world_color, coord+ivec2(xmir, 1));
		imageStore(world_dead, coord, uvec4(c, 0, 0, 0));
		imageStore(world_color, coord, e);
		imageStore(world_dead, coord+ivec2(xmir,1), uvec4(a, 0, 0, 0));
		imageStore(world_color, coord+ivec2(xmir,1), d);
	}
}