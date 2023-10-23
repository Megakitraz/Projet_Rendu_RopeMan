#include "player.hpp"

KeyMapping PLAYERS_MAPPING_QWERTY = 
	{GLFW_KEY_LEFT,GLFW_KEY_RIGHT,GLFW_KEY_UP,GLFW_KEY_DOWN,GLFW_KEY_SPACE};



Player::Player()
{
	m_radius = 1.0f;//meter
	m_mass = 1.0f;
	m_ground_friction = 0.1f;
	m_fluid_friction = 0.05f;
	m_bounce_coef = 0.5f;
	m_jump_intensity = 1.0f;
	m_input_intensity = 1.0f;
	m_rope_throw_intensity = 1.0f;
	m_local_left =  vec3(-1.0f, 0.0f, 0.0f);
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