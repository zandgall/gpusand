#version 460 compatibility

#define X_SIZE 16
#define Y_SIZE 16

layout (local_size_x = X_SIZE, local_size_y = Y_SIZE, local_size_z = 1) in;


layout (r32ui, binding = 0) uniform uimage2D world_dead;
layout (rgba32f, binding = 1) uniform image2D world_color;
shared uint dead_shared[64*64];

layout (r32ui, location = 4) uniform readonly uimage1D gravity_affected;
layout (r32ui, location = 5) uniform readonly uimage1D non_solid;

bool match(uint a, readonly uimage1D img) {
	for(int i = 0, n = imageSize(img); i < n; i++)
		if(imageLoad(img, i).r == a)
			return true;
	return false;
}

void main() {
	if(gl_LocalInvocationIndex==0) {
		for(int j = 0; j < 64; j++)
			for(int i = 0; i < 64; i++)
				dead_shared[i+j*64] = imageLoad(world_dead, ivec2(i,j)).r;

	}

	ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
	uint a = dead_shared[coord.x+coord.y*64], b = dead_shared[coord.x+(coord.y+1)*64];
	if(match(a, gravity_affected) && match(b, non_solid)) {
		vec4 c = imageLoad(world_color, coord), d = imageLoad(world_color, coord+ivec2(0, 1));
		dead_shared[coord.x+coord.y*64] = b;
		imageStore(world_color, coord, d);
		dead_shared[coord.x+(coord.y+1)*64] = a;
		imageStore(world_color, coord+ivec2(0,1), c);
	}

	if(gl_LocalInvocationIndex==X_SIZE*Y_SIZE-1) {
		for(int j = 0; j < 64; j++)
			for(int i = 0; i < 64; i++)
				imageStore(world_dead, ivec2(i,j), uvec4(dead_shared[i+j*64], 0, 0, 0));

	}
}