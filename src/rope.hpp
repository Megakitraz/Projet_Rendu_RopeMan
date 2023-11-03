#ifndef ROPE_CLASS
#define ROPE_CLASS

#include"buffer_structures.hpp"
#include "helpers/helpers_common.hpp"
#include <GLFW/glfw3.h>

class Player;
class Rope {

public:
	Rope(std::uint32_t control_point_count, Player& player);
	//~Rope();
	void compute_p_shading();
	void compute_tangents();
	void compute_rope_physics(float delta_time, float gravity);

	static void gui();

	//In public attribute to make it easier
	vec4* p_shading;//position with offset
	vec4* t_shading;//tangents
	vec3* p;//position
	vec3* v;//velocity
	vec3* a;//acceleration
	Player& m_player;
private:
	bool is_head_bound_to_platform;
	bool is_tail_bound_to_player;
	bool is_left_hand;
	float segment_length; //longueur d'un segment 
	float segment_mass;   //masse d'un noeud entre deux segment
	float segment_k;		 //constante de raideur de chaque segment
	std::uint32_t m_control_point_count;
	
	int m_id;

};

#endif

