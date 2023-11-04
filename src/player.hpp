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

class ProjectionMatrix {

public:
	ProjectionMatrix();
	void set_viewport_resolution(glm::uvec2 res);
	void set_perspective(float fov_y_degree, float near, float far);
	void set_ortho_centered(float size_y, float near, float far);

	glm::mat4 m_proj;

private:
	float m_ratio_x_over_y;
	glm::uvec2 m_resolution;
	bool m_is_perspective;
	CacheCameraParams m_params;

};

class Player {

public:
	Player();
	void init(); // incomplete
	void poll_keys_mouse();
	void write_params_to_application_struct(ApplicationUboDataStructure& app_ubo);
	void gui();
	void Move();
	void AddAcceleration(vec3 acc);
	

	//In public attribute to make it easier
	static float m_mass;//Mass of player
	vec3 p;//pos
	vec3 v;//velocity
	vec3 a;//acceleration
	vec3 m_local_left;//ws
	float m_radius;//PLayer is a modeled as a sphere (for collision detextion, rope throw offset)
	glm::mat4 m_w_v;//world view matrix


private:
	

	float m_ground_friction;//f_g = m_ground_friction in direction opposing acceleration if on ground
	float m_fluid_friction;// f_f = m_fluid_friction * velocity in direction opposing acceleration
	float m_bounce_coef;//When hitting a platform, "wall" component of speed is reversed with that factor 
	float m_jump_intensity;// have to be decided : jump only from ground? acceleration / velocity based ?
	float m_input_intensity;// strength of keyboard input
	float m_rope_throw_intensity;// how far a rope is launched, probably initial acceleration !
	bool m_lock_mouse_mode;
	vec2 m_mouse_coords;
	float m_theta;
	float m_phi;
	float m_mouse_radians_per_pixel;
	float m_speed_unit_sec;
	vec3 m_forward;
	vec3 m_right;
	vec3 m_up;

	

	

	float gravityStrength;


	void build_basis();

	
	


};

#endif

