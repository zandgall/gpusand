#shader compute
#version 460 compatibility

layout (local_size_x = 5, local_size_y = 1, local_size_z = 1) in;

layout (rgba32f, binding = 0) uniform writeonly image2D world_color;
layout (r32ui, binding = 1) uniform uimage2D world_dead;

