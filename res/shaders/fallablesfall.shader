#version 460 compatibility

layout (local_size_x = 32, local_size_y = 32, local_size_z = 1) in;

layout (rgba32f, binding = 0) uniform image2D world_color;
layout (r32ui, binding = 1) uniform uimage2D world_dead;

layout (r32ui, location = 0) uniform readonly uimage1D gravity_affected;
layout (r32ui, location = 1) uniform readonly uimage1D non_solid;

bool match(uint a, readonly uimage1D img) {
	for(int i = 0, n = imageSize(img); i < n; i++)
		if(imageLoad(img, i).r == a)
			return true;
	return false;
}

void main() {
	ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
	uint a = imageLoad(world_dead, coord).r, b = imageLoad(world_dead, coord+ivec2(0,1)).r;
	if(match(a, gravity_affected) && match(b, non_solid)) {
		vec4 c = imageLoad(world_color, coord), d = imageLoad(world_color, coord+ivec2(0, 1));
		imageStore(world_dead, coord, uvec4(b, 0, 0, 0));
		imageStore(world_color, coord, d);
		imageStore(world_dead, coord+ivec2(0,1), uvec4(a, 0, 0, 0));
		imageStore(world_color, coord+ivec2(0,1), c);
	}
}