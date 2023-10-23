#version 460
#define UBO_APPLICATION_BINDING 0
layout (location = 0) out vec4 pixel_color;

layout(binding = UBO_APPLICATION_BINDING, std140) uniform UBO_APPLICATION
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

out vec3 pos_ws;
void main() 
{
    float lava_square_size = 40000.0f;//something big

    //creates a big square around player
    if(gl_VertexID==0)
    {
        pos_ws = vec3(player_pos.x - lava_square_size,lava_params.x,player_pos.z - lava_square_size);
        gl_Position = w_v_p * vec4(pos_ws,1.0);
    }
    else if(gl_VertexID==1)
    {
        pos_ws = vec3(player_pos.x + lava_square_size,lava_params.x,player_pos.z - lava_square_size);
        gl_Position = w_v_p * vec4(pos_ws,1.0);
    }
    else if(gl_VertexID==2)
    {
        pos_ws = vec3(player_pos.x - lava_square_size,lava_params.x,player_pos.z + lava_square_size);
        gl_Position = w_v_p * vec4(pos_ws,1.0);
    }
    else //(gl_VertexID==3)
    {
        pos_ws = vec3(player_pos.x + lava_square_size,lava_params.x,player_pos.z + lava_square_size);
        gl_Position = w_v_p * vec4(pos_ws,1.0);
    }
}

