#ifndef PLAYER_CLASS
#define PLAYER_CLASS

#include"buffer_structures.hpp"
#include "helpers/helpers_common.hpp"
#include <GLFW/glfw3.h>

struct KeyMapping {
	int left;
	int right;
	int up;
	int down;
	int jump;
	//dive ?
};


extern KeyMapping PLAYERS_MAPPING_QWERTY;


class Player {

public:
	Player();
	void init(); // incomplete
	void poll_keys_mouse();
	void write_params_to_application_struct(ApplicationUboDataStructure& app_ubo);
	void gui();

	//In public attribute to make it easier
	float m_mass;//Mass of player
	vec3 p;//pos
	vec3 v;//velocity
	vec3 a;//acceleration
	vec3 m_local_left;//ws
	float m_radius;//PLayer is a modeled as a sphere (for collision detextion, rope throw offset)
private:
	

	float m_ground_friction;//f_g = m_ground_friction in direction opposing acceleration if on ground
	float m_fluid_friction;// f_f = m_fluid_friction * velocity in direction opposing acceleration
	float m_bounce_coef;//When hitting a platform, "wall" component of speed is reversed with that factor 
	float m_jump_intensity;// have to be decided : jump only from ground? acceleration / velocity based ?
	float m_input_intensity;// strength of keyboard input
	float m_rope_throw_intensity;// how far a rope is launched, probably initial acceleration !
	


};

#endif

