#include "player.hpp"

KeyMapping PLAYERS_MAPPING_QWERTY = 
	{GLFW_KEY_LEFT,GLFW_KEY_RIGHT,GLFW_KEY_UP,GLFW_KEY_DOWN,GLFW_KEY_SPACE};

ProjectionMatrix::ProjectionMatrix()
{
	m_ratio_x_over_y = -1.0f;
	m_is_perspective = true;
	m_resolution = glm::uvec2(0);
	m_params = { 0.0f };
	m_proj = glm::mat4(0.0f);
}

void ProjectionMatrix::set_viewport_resolution(glm::uvec2 res)
{
	if (res.x == 0 || res.y == 0)
		return;
	if (res.x != m_resolution.x || res.y != m_resolution.y || m_ratio_x_over_y == -1.0f)
	{
		if (m_ratio_x_over_y != -1.0f)
		{
			m_resolution = res;
			m_ratio_x_over_y = (float)(res.x) / (float)(res.y);
			if (m_is_perspective)
				set_perspective(m_params.fov_y_degree, m_params.z_near, m_params.z_far);
			else
				set_ortho_centered(m_params.size_y, m_params.z_near, m_params.z_far);

		}
		else
		{
			m_resolution = res;
			m_ratio_x_over_y = (float)(res.x) / (float)(res.y);
		}
	}
}

void ProjectionMatrix::set_perspective(float fov_y_degree, float near, float far)
{
	m_is_perspective = true;
	m_params.fov_y_degree = fov_y_degree;
	m_params.z_near = near;
	m_params.z_far = far;

	m_proj = glm::perspective(glm::radians(fov_y_degree), m_ratio_x_over_y, near, far);

}

void ProjectionMatrix::set_ortho_centered(float size_y, float near, float far)
{
	m_is_perspective = false;
	m_params.size_y = size_y;
	m_params.z_near = near;
	m_params.z_far = far;
	m_proj = glm::ortho(-size_y * m_ratio_x_over_y, size_y * m_ratio_x_over_y, -size_y, size_y, near, far);

}



Player::Player()
{
	m_radius = 1.0f;//meter
	//m_mass = 1.0f;
	m_ground_friction = 0.1f;
	m_fluid_friction = 0.05f;
	m_bounce_coef = 0.5f;
	m_jump_intensity = 1.0f;
	m_input_intensity = 1.0f;
	m_rope_throw_intensity = 1.0f;
	m_local_left =  vec3(-1.0f, 0.0f, 0.0f);
	m_lock_mouse_mode = false;
	m_mouse_radians_per_pixel = 5.f;
	m_up = glm::vec3(0.0f, 1.0f, 0.0f);
	gravityStrength = 9.81f;
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
	m_phi = glm::clamp(m_phi + delta_mouse.y * m_mouse_radians_per_pixel, -3.13f * 0.5f, 3.13f * 0.5f);


	if (glfwGetKey(ContextHelper::window, GLFW_KEY_W) == GLFW_PRESS)
		v.x +=( m_speed_unit_sec * m_forward).x;
		v.z += (m_speed_unit_sec * m_forward).z;
	if (glfwGetKey(ContextHelper::window, GLFW_KEY_S) == GLFW_PRESS)
		v.x -= (m_speed_unit_sec * m_forward).x;
		v.z -= (m_speed_unit_sec * m_forward).z;
	if (glfwGetKey(ContextHelper::window, GLFW_KEY_A) == GLFW_PRESS)
		v.x -= (m_speed_unit_sec * m_right).x;
		v.z -= (m_speed_unit_sec * m_right).z;
	if (glfwGetKey(ContextHelper::window, GLFW_KEY_D) == GLFW_PRESS)
		v.x += (m_speed_unit_sec * m_right).x;
		v.z += (m_speed_unit_sec * m_right).z;

	build_basis();
}

void Player::Move()
{
	a -= gravityStrength * m_up;
	v += a * ContextHelper::time_frame_s;
	p += v * ContextHelper::time_frame_s;
	a -= a; // a = 0

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

}

void Player::write_params_to_application_struct(ApplicationUboDataStructure& app_ubo)
{
	//app_ubo.player_color[m_id] = m_color;
	//app_ubo.player_direction[m_id] = m_direction;
}

void Player::gui()
{
	if (ImGui::TreeNode("Player parameters"))
	{
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