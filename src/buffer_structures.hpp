#ifndef BUFFER_STRUCTURES
#define BUFFER_STRUCTURES

#include <glm/glm.hpp>

#define uint uint32_t // for compatibility with GLSL
using namespace glm;

//structures for uniform buffers (UBOs) require proper 16 byte alignment (std140 layout)
//structures for shader storage buffers (SSBOs) require only 4 byte alignment (std430 layout)
//more at https://www.khronos.org/opengl/wiki/Interface_Block_(GLSL)#Memory_layout

//Texture channels ID:
#define TEXTURE_ENVIRONMENT 0//cubemap

//GLSL: layout(binding = 0, std140) uniform UBO_APPLICATION
#define UBO_APPLICATION_BINDING 0
struct ApplicationUboDataStructure
{
	//Matrices fed from player camera
	mat4 proj; //projection matrix (view to eye)
	mat4 inv_proj; //inverse projection matrix (eye to view)
	mat4 w_v; //world to view matrix
	mat4 w_v_p; //world to eye matrix
	mat4 inv_w_v_p; //eye to world matrix
	//Player
	vec4 player_pos;//.xyz: player.p, .w: time
	//Ropes
	vec4 rope_color; //.xyz: color, .w: rope radius
	//Lava
	vec4 lava_params;//.x: height, .y: light intensity, zw: unused
	//Platforms
	vec4 platform_cell_length; //.x:cell_length.x, .y:cell_length.y, .z:cell_length.z,  .w: empty_likelyhood
	vec4 platform_cell_offset; //.x:cell_offset.x, .y:cell_offset.y, .z:cell_offset.z, .w: unused
	vec4 platform_length;//.x: min_length_xz, .y: max_length_xz, .z: min_length_y, .w: max_length_z
	//Light
	vec4 left_rope_light; //.xyz: pos, .w: intensity
	vec4 right_rope_light; //.xyz: pos, .w: intensity
};

#endif

