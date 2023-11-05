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

in vec3 dir_ws;

#define MAX_STEPS 256
#define MIN_DISTANCE 0.01
#define MAX_DISTANCE 10000







float sdf_box( vec3 pos, vec3 center,vec3 half_size, float round_radius );
vec3 get_platform_normal(vec3 p);
float get_ndc_depth(vec3 intersection);

float hash14(vec4 p4);
vec3 hash34(vec4 p4);

float platforms_sdf(vec3 pos)
{

//which orange box we belong (sketch on discord)
ivec3 cell_id_3d = ivec3(floor(pos / platform_cell_length.xyz));//this per-cell "seed" will drive random generation
//cell_id_3d = ivec3(3,3,3);


vec3 bounding_box_cell_half_length = platform_cell_length.xyz*0.5 + platform_cell_offset.xyz;
vec3 bounding_box_cell_center = (cell_id_3d+0.5)*platform_cell_length.xyz;

//This is the red box (sketch on discord)
float sdf_bounding_box = sdf_box(pos,bounding_box_cell_center,bounding_box_cell_half_length,0.0);

//green platform, must be inside the green box
//Here, centered in middle of cell, to be randomized in length, width, position, 
//but remaining in the green box  = orange box - platform_cell_offset.xyz (all sides)


//For random calls in [0,1] use hash14 or hash34 (for 3 numbers), and increment the forth coord
float sdf_platform = sdf_box(pos,bounding_box_cell_center,platform_length.ywy*0.5,3.0);

//hash 14 called with a seed made of the cell 3d ID + a "call ID" = 0.0 (to be incremented)
if (hash14(vec4(cell_id_3d,0.0))<platform_cell_length.w)
    sdf_platform = 99999999.9;


//the SDF is the union between the plaform and the complementary of the bounding box
return min(-sdf_bounding_box,sdf_platform);
}

void main() 
{
    //Classic raymarcher (like TP3)
    vec3 ori = player_pos.xyz;
    vec3 dir = normalize(dir_ws);
    float t=0.0;//distance to origin, in ray dir
    
    vec3 platform_pos = ori;
    for (int i = 0; i < MAX_STEPS;i++)
    {
        float d = platforms_sdf(platform_pos);
        t += d;
        platform_pos = ori + t * dir;
        if (d<MIN_DISTANCE) // reached surface
        {
            break;//exit loop
        }
        else if (t>MAX_DISTANCE || i==MAX_STEPS-1)//too far: background
        {
            discard;//delete pixel
            return;//exit shader
        }
        
    }
    //if we reached here, platform_pos is set to the surface of the platform, now we shade

    vec3 n = get_platform_normal(platform_pos);
    
    //phong shading ?

    gl_FragDepth = get_ndc_depth(platform_pos); // write correct depth in z-buffer



    pixel_color = vec4(0.5+ 0.5 *n,1.0);
}


vec3 get_platform_normal(vec3 p) 
{
    const float h = 0.001;
    const vec2 k = vec2(1.0,-1.0);
    return normalize( k.xyy*platforms_sdf( p + k.xyy*h) + 
                      k.yyx*platforms_sdf( p + k.yyx*h) + 
                      k.yxy*platforms_sdf( p + k.yxy*h) + 
                      k.xxx*platforms_sdf( p + k.xxx*h) );
}


float get_ndc_depth(vec3 intersection)
{
float z_vs = dot(intersection-player_pos.xyz,vec3(w_v[0].z,w_v[1].z,w_v[2].z));
return 0.5+0.5*(z_vs*proj[2][2]+proj[3][2])/z_vs;
}


float sdf_box( vec3 pos, vec3 center,vec3 half_size, float round_radius )
{
    vec3 q = abs(pos - center) - half_size+round_radius;
    return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0) - round_radius;
}

float hash14(vec4 p4)
{
    p4 = fract(p4  * vec4(.1031, .1030, .0973, .1099));
    p4 += dot(p4, p4.wzxy+33.33);
    return fract((p4.x + p4.y) * (p4.z + p4.w));
}

vec3 hash34(vec4 p4)
{
    p4 = fract(p4  * vec4(.1031, .1030, .0973, .1099));
    p4 += dot(p4, p4.wzxy+33.33);
    return fract((p4.xxyz+p4.yzzw)*p4.zywx).xyz;
}