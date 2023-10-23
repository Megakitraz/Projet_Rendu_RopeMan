#version 460

layout (location = 0) out vec4 pixel_color;

in vec3 dir_ws;

layout(binding = 0) uniform samplerCube envmap;

void main() 
{
    pixel_color = vec4(texture(envmap,dir_ws).rgb,1.0);
    gl_FragDepth = 1.0;//Force max depth
}

