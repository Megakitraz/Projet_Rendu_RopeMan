#ifndef GAME_CLASS
#define GAME_CLASS

#include "player.hpp"
#include "rope.hpp"
#include"buffer_structures.hpp"
#include "helpers/helpers_common.hpp"
#include <GLFW/glfw3.h>


class Game {//One instance

public:
	Game();
	void load_shaders(std::string base_path);
	void load_cubemap(std::string base_path, std::string base_name);

	void init_game();//To call when (re-) starting a game

	void write_ropes_data_to_vbos();
	void write_params_to_application_struct(ApplicationUboDataStructure& app_ubo);

	void draw_lava();
	void draw_platforms();
	void draw_environment();
	void draw_ropes();
	void gui(ApplicationUboDataStructure& app_ubo);
	float m_gravity_amplitude;//in direction -y
	float m_delta_t_s;//simulation time-step in second
	Player m_player;

private:

	//Drawing shaders
	ShaderGLSL* m_shader_rope_body;//drawing the ropes, we start with something very simple
	//ShaderGLSL* m_shader_rope_head;//If time, we will draw a grapple at the head of the rope
	ShaderGLSL* m_shader_environment;//cubemap or procedural
	ShaderGLSL* m_shader_lava;//shader for lava, cool stuff can be made here 
	ShaderGLSL* m_shader_platforms;//ray-marcher + cool effects ?
	//PLayer is not drawn, since First person !

	//Ressources
	VertexArrayObject m_vao_dummy;
	VertexArrayObject m_vao_ropes;
	Cubemap m_tex_environment;

	//Rope manager
	std::uint32_t m_rope_bank_size;//number of ropes managed
	std::uint32_t m_control_point_per_rope_count;
	std::vector<Rope> m_rope_bank;
		//GPU rope ressources
		GPUBuffer m_rope_bank_pos_VBO;//VBO storing postion of all ropes control points
		GPUBuffer m_rope_bank_tan_VBO;//VBO storing tangent of all ropes control points
		std::uint32_t m_size_of_rope;// = m_control_point_per_rope_count * sizeof(vec4)

	float m_lava_altitude;//in Y dimension
	float m_lava_speed;

	//Platforms
	vec4 m_platform_cell_length;//.x:cell_length.x, .y:cell_length.y, .z:cell_length.z,  .w: empty_likelyhood
	vec4 m_platform_cell_offset; //.x:cell_offset.x, .y:cell_offset.y, .z:cell_offset.z, .w: unused
	vec4 m_platform_length;//.x: min_length_xz, .y: max_length_xz, .z: min_length_y, .w: max_length_z
	// 2  * platform_cell_offset + platform_max_length < platform_cell_length !


	//parameters not exposed in GUI
	int m_game_status;//0 = not playing, 1 = playing
	float m_time_game_s;

};

#endif

