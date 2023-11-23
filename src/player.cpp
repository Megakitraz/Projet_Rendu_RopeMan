#include "player.hpp"*
#include "game.hpp"

KeyMapping PLAYERS_MAPPING_QWERTY = 
	{GLFW_KEY_LEFT,GLFW_KEY_RIGHT,GLFW_KEY_UP,GLFW_KEY_DOWN,GLFW_KEY_SPACE};

#define MAX_STEPS 256
#define MIN_DISTANCE 0.01
#define MAX_DISTANCE 10000



Player::Player()
{
	p = vec3(0.0f);
	v= vec3(0.0f);
	a = vec3(0.0f);
	m_radius = 1.0f;//meter
	m_mass = 1.0f;
	m_ground_friction = 0.1f;
	m_fluid_friction = 0.05f;
	m_bounce_coef = 0.5f;
	m_jump_intensity = 1.0f;
	m_input_intensity = 1.0f;
	m_rope_throw_intensity = 1.0f;
	m_local_left =  vec3(-1.0f, 0.0f, 0.0f);
	m_lock_mouse_mode = false;
	m_w_v = glm::mat4(1.0f);
	m_mouse_radians_per_pixel = 5.f;
	m_up = glm::vec3(0.0f, 1.0f, 0.0f);
	m_forward = glm::vec3(1.0f, 0.0f, 0.0f);
	m_right = glm::vec3(0.0f, 0.0f, 1.0f);
	m_theta = 0.0f;
	m_phi = 0.0f;
	m_mouse_coords = glm::vec2(0.0f);
	m_speed_unit_sec = 1.0f;
	gravityStrength = 0.f;//9.81
	time_step = 1.f/120.f;
	speed = 100;
	platform_cell_length = vec4(0.0f);
	platform_cell_offset = vec4(0.0f);
	platform_length = vec3(0.0f);
}



void Player::poll_keys_mouse()//Incomplete function
{
	float lr = 0.0f;//1.0: right, -1.0: left //x_ws axis
	float ud = 0.0f;//1.0: up, -1.0: down //z_ws axis

	if (glfwGetKey(ContextHelper::window, PLAYERS_MAPPING_QWERTY.left) == GLFW_PRESS)
		lr = -1.0f;
	if (glfwGetKey(ContextHelper::window, PLAYERS_MAPPING_QWERTY.right) == GLFW_PRESS)
		lr = 1.0f;
	if (glfwGetKey(ContextHelper::window, PLAYERS_MAPPING_QWERTY.up) == GLFW_PRESS)
		ud = 1.0f;
	if (glfwGetKey(ContextHelper::window, PLAYERS_MAPPING_QWERTY.down) == GLFW_PRESS)
		ud = -1.0f;

	/*
	if (lr != 0.0f || ud != 0.0f)//avoid normalizing null vector !
		m_direction = vec4(lr, 0.0f, ud, 1.0f);
	else
		m_direction = vec4(0.0f, 0.0f, 0.0f, 1.0f);*/

	static int lock_key_state_previous = GLFW_RELEASE;
	int lock_key_state = glfwGetKey(ContextHelper::window, GLFW_KEY_F);
	if (lock_key_state == GLFW_PRESS && lock_key_state_previous == GLFW_RELEASE)
	{
		m_lock_mouse_mode = !m_lock_mouse_mode;
	}
	lock_key_state_previous = lock_key_state;

	glm::vec2 delta_mouse = glm::vec2(0.0f);
	double mouse_x, mouse_y;
	glfwGetCursorPos(ContextHelper::window, &mouse_x, &mouse_y);
	if (m_lock_mouse_mode)
	{
		glfwSetInputMode(ContextHelper::window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

		glfwSetCursorPos(ContextHelper::window, glm::floor(0.5 * (double)ContextHelper::resolution.x), glm::floor(0.5 * (double)ContextHelper::resolution.y));

		delta_mouse.x = m_mouse_coords.x - (float)mouse_x;
		delta_mouse.y = m_mouse_coords.y - (float)mouse_y;
		m_mouse_coords.x = glm::floor(0.5f * (float)ContextHelper::resolution.x);
		m_mouse_coords.y = glm::floor(0.5f * (float)ContextHelper::resolution.y);
	}
	else
	{
		glfwSetInputMode(ContextHelper::window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		if (glfwGetMouseButton(ContextHelper::window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		{
			delta_mouse.x = (float)mouse_x - m_mouse_coords.x;
			delta_mouse.y = (float)mouse_y - m_mouse_coords.y;
		}
		m_mouse_coords.x = (float)mouse_x;
		m_mouse_coords.y = (float)mouse_y;
	}

	m_theta += delta_mouse.x * m_mouse_radians_per_pixel;
	m_phi = glm::clamp(m_phi  - delta_mouse.y * m_mouse_radians_per_pixel, -3.13f * 0.5f, 3.13f * 0.5f);

	if (glfwGetKey(ContextHelper::window, GLFW_KEY_W) == GLFW_PRESS)
		{
			p.x -= (m_speed_unit_sec * m_forward).x * time_step * speed;
			p.z -= (m_speed_unit_sec * m_forward).z * time_step * speed;
		}
	if (glfwGetKey(ContextHelper::window, GLFW_KEY_S) == GLFW_PRESS)
		{
			p.x += (m_speed_unit_sec * m_forward).x * time_step * speed;
			p.z += (m_speed_unit_sec * m_forward).z * time_step * speed;
		}
	if (glfwGetKey(ContextHelper::window, GLFW_KEY_A) == GLFW_PRESS)
		{
			p.x += (m_speed_unit_sec * m_right).x * time_step * speed;
			p.z += (m_speed_unit_sec * m_right).z * time_step * speed;
		}
	if (glfwGetKey(ContextHelper::window, GLFW_KEY_D) == GLFW_PRESS)
		{
			p.x -= (m_speed_unit_sec * m_right).x * time_step * speed;
			p.z -= (m_speed_unit_sec * m_right).z * time_step * speed;
		}

	build_basis();
}

void Player::Move()
{
	a -= gravityStrength * m_up;
	v += a * time_step - v * m_ground_friction * time_step;
	if (v != vec3(0.0f))
	{
		p = CollisionResult(p, glm::normalize(v), sqrt(dot(v, v) * time_step));
	}
	
	a = vec3(0.0f);
	build_basis();

}

vec3 Player::CollisionResult(vec3 origin, vec3 direction, float dist)
{
	//Classic raymarcher (like TP3)
	float t = 0.0;//distance to origin, in ray dir

	vec3 platform_pos = origin;
	for (int i = 0; i < MAX_STEPS; i++)
	{
		float d = platforms_sdf(platform_pos);
		t += d;
		platform_pos = origin + t * direction;
		if (d < MIN_DISTANCE) // reached surface
		{
			v = vec3(0.0f);
			a = vec3(0.0f);
			return platform_pos;
		}
		else if (t > dist || i == MAX_STEPS - 1)//too far
		{
			return p + direction * dist;
		}

	}
}

void Player::AddAcceleration(vec3 acc)
{
	a += acc;
}

void Player::build_basis()
{
	float ct = glm::cos(m_theta);
	float st = glm::sin(m_theta);
	float cp = glm::cos(m_phi);
	float sp = glm::sin(m_phi);

	m_forward.x = ct * cp;
	m_forward.y = sp;
	m_forward.z = st * cp;
	m_right = glm::normalize(glm::cross(m_up, m_forward));

	m_w_v = glm::lookAt(p, p + m_forward, m_up);
}

void Player::init()
{
	set_camera(vec3(-35.0f, 30.0f, -30.0f), 0.f, 0.f);
	set_params(0.1f, 10.0f);
}

void Player::write_params_to_application_struct(ApplicationUboDataStructure& app_ubo)
{
	//app_ubo.player_color[m_id] = m_color;
	//app_ubo.player_direction[m_id] = m_direction;
}

void Player::set_camera(const glm::vec3 position, float theta, float phi)
{
	p = position;
	m_theta = glm::radians(theta);
	m_phi = glm::radians(phi);
	build_basis();

	double mouse_x, mouse_y;
	glfwGetCursorPos(ContextHelper::window, &mouse_x, &mouse_y);
	m_mouse_coords = glm::vec2((float)mouse_x, (float)mouse_y);
}

void Player::set_params(const float mouse_degree_per_pixel, const float speed_unit_sec)
{
	m_mouse_radians_per_pixel = glm::radians(mouse_degree_per_pixel);
	m_speed_unit_sec = speed_unit_sec;
}


float Player::platforms_sdf(vec3 pos)
{

	//which orange box we belong (sketch on discord)
	ivec3 cell_id_3d = ivec3(floor(pos.x / platform_cell_length.x), floor(pos.y / platform_cell_length.y), floor(pos.z / platform_cell_length.z));//this per-cell "seed" will drive random generation
	//cell_id_3d = ivec3(3,3,3);


	vec3 bounding_box_cell_half_length = vec3(platform_cell_length.x, platform_cell_length.y, platform_cell_length.z) * 0.5f + vec3(platform_cell_offset.x, platform_cell_offset.y, platform_cell_offset.z); // red box
	vec3 bounding_box_cell_center = vec3( cell_id_3d.x + 0.5f, cell_id_3d.y + 0.5f, cell_id_3d.z + 0.5f) * vec3 (platform_cell_length.x, platform_cell_length.y, platform_cell_length.z);

	//This is the red box (sketch on discord)
	float sdf_bounding_box = sdf_box(pos, bounding_box_cell_center, bounding_box_cell_half_length, 0.0);

	//green platform, must be inside the green box
	//Here, centered in middle of cell, to be randomized in length, width, position, 
	//but remaining in the green box  = orange box - platform_cell_offset.xyz (all sides)
	vec3 green_box = vec3(platform_cell_length.x, platform_cell_length.y, platform_cell_length.z) * 0.5f - vec3(platform_cell_offset.x, platform_cell_offset.y, platform_cell_offset.z);



	//For random calls in [0,1] use hash14 or hash34 (for 3 numbers), and increment the forth coord
	float sdf_platform = sdf_box(pos, bounding_box_cell_center + (hash34(vec4(cell_id_3d, 0.0)) * 2.0f - 1.0f) * green_box, vec3(platform_length.x, platform_length.y, platform_length.z) * 0.5f, 3.0);

	//hash 14 called with a seed made of the cell 3d ID + a "call ID" = 0.0 (to be incremented)
	if (hash14(vec4(cell_id_3d, 0.0)) < platform_cell_length.w)
		sdf_platform = 99999999.9;


	//the SDF is the union between the plaform and the complementary of the bounding box
	return min(-sdf_bounding_box, sdf_platform);
}


float Player::sdf_box(vec3 pos, vec3 center, vec3 half_size, float round_radius)
{
	vec3 q = abs(pos - center) - half_size + round_radius;
	return length( vec3(max(q.x, 0.0f), max(0.0f, q.y), max(0.0f, q.z))) + min(max(q.x, max(q.y, q.z)), 0.0f) - round_radius;
}

float Player::hash14(vec4 p4)
{
	p4 = fract(p4 * vec4(.1031, .1030, .0973, .1099));
	p4 += dot(p4, vec4(p4.w +33.33f, p4.z +33.33f, p4.x +33.33f, p4.y +33.33f));
	return fract((p4.x + p4.y) * (p4.z + p4.w));
}

vec3 Player::hash34(vec4 p4)
{
	p4 = fract(p4 * vec4(.1031, .1030, .0973, .1099));
	p4 += dot(p4, vec4(p4.w + 33.33f, p4.z + 33.33f, p4.x + 33.33f, p4.y + 33.33f));
	return vec3(
		fract(( vec4(p4.x, p4.x, p4.y, p4.z) + (vec4(p4.y, p4.z, p4.z, p4.w)) ) * (vec4(p4.z, p4.y, p4.w, p4.x))).x,
		fract((vec4(p4.x, p4.x, p4.y, p4.z) + (vec4(p4.y, p4.z, p4.z, p4.w))) * (vec4(p4.z, p4.y, p4.w, p4.x))).y,
		fract((vec4(p4.x, p4.x, p4.y, p4.z) + (vec4(p4.y, p4.z, p4.z, p4.w))) * (vec4(p4.z, p4.y, p4.w, p4.x))).z);
}

void Player::gui()
{
	if (ImGui::TreeNode("Player parameters"))
	{
		//ImGui::SliderInt("Player Speed", &speed, 1, 200);
		ImGui::SliderFloat("Collision radius", &m_radius, 0.5f, 2.0f);
		ImGui::SliderFloat("Mass", &m_mass, 0.5f, 2.0f);
		ImGui::SliderFloat("Ground friction", &m_ground_friction, 0.0f, 1.0f);
		ImGui::SliderFloat("Fluid friction", &m_fluid_friction, 0.01f,0.1f);
		ImGui::SliderFloat("Bounce coefficient", &m_bounce_coef, 0.0f, 1.0f);
		ImGui::SliderFloat("Jump intensity", &m_jump_intensity, 0.0f, 2.0f);
		ImGui::SliderFloat("Input intensity", &m_input_intensity, 0.0f, 2.0f);
		ImGui::SliderFloat("Rope throw intensity", &m_rope_throw_intensity, 0.0f, 2.0f);
	ImGui::TreePop();
	}
}