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
	void Move();
	void AddAcceleration(vec3 acc);

	void set_camera(const glm::vec3 position, float theta, float phi);
	void set_params(const float mouse_degree_per_pixel, const float speed_unit_sec);
	

	//In public attribute to make it easier
	float m_mass;//Mass of player
	vec3 p;//pos
	vec3 v;//velocity
	vec3 a;//acceleration
	vec3 m_local_left;//ws
	float m_radius;//PLayer is a modeled as a sphere (for collision detextion, rope throw offset)
	glm::mat4 m_w_v;//world view matrix
	float time_step;
	float m_theta;
	float m_phi;
	vec3 m_forward;
	vec3 m_right;
	vec3 m_up;

	int speed;

	vec3 CollisionResult(vec3 origin, vec3 direction, float dist);

	float sdf_box(vec3 pos, vec3 center, vec3 half_size, float round_radius);
	float hash14(vec4 p4);
	vec3 hash34(vec4 p4);
	float platforms_sdf(vec3 pos);

	vec4 platform_cell_length;
	vec4 platform_cell_offset;
	vec3 platform_length;

	float m_rope_throw_intensity;// how far a rope is launched, probably initial acceleration !


private:
	

	float m_ground_friction;//f_g = m_ground_friction in direction opposing acceleration if on ground
	float m_fluid_friction;// f_f = m_fluid_friction * velocity in direction opposing acceleration
	float m_bounce_coef;//When hitting a platform, "wall" component of speed is reversed with that factor 
	float m_jump_intensity;// have to be decided : jump only from ground? acceleration / velocity based ?
	float m_input_intensity;// strength of keyboard input
	
	bool m_lock_mouse_mode;
	vec2 m_mouse_coords;

	float m_mouse_radians_per_pixel;
	float m_speed_unit_sec;
	

	

	

	float gravityStrength;


	void build_basis();

	
	


};

#endif

