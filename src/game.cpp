#include "game.hpp"

Game::Game()
{
	m_shader_rope_body = new ShaderGLSL("shader_rope_body");
	m_shader_environment = new ShaderGLSL("shader_environment");
	m_shader_lava = new ShaderGLSL("shader_lava");
	m_shader_platforms = new ShaderGLSL("shader_platforms");

	m_control_point_per_rope_count = 10u;
	m_rope_bank_size = 20u;
	m_rope_bank.resize(m_rope_bank_size,Rope(m_control_point_per_rope_count,m_player));

	m_gravity_amplitude = 1.0f;
	m_delta_t_s = 0.002f;

	m_rope_bank_pos_VBO.allocate(4);//Will be re-allocated
	m_rope_bank_tan_VBO.allocate(4);

	m_platform_cell_length = vec4(200.0f, 50.0f, 200.0f,0.2f);
	m_platform_cell_offset = vec4(100.0,25.0f, 100.0f, 0.2f);
	m_platform_length = vec4(40.0f, 20.0f, 40.0f, 20.0f);
	init_game();
}


void Game::load_cubemap(std::string base_path, std::string base_name)
{
	m_tex_environment.set_format_params({ GL_RGB8,GL_RGB ,GL_UNSIGNED_BYTE ,3 });
	m_tex_environment.create_from_file((base_path + "data/"+ base_name + "/").c_str(), ".jpg", false);
	m_tex_environment.set_filtering_params();
	m_tex_environment.set_slot(TEXTURE_ENVIRONMENT);
}


void Game::load_shaders(std::string base_path)
{
	m_shader_rope_body->add_shader(GL_VERTEX_SHADER, base_path, "shaders/rope_body_vs.glsl");
	m_shader_rope_body->add_shader(GL_FRAGMENT_SHADER, base_path, "shaders/rope_body_fs.glsl");
	m_shader_rope_body->compile_and_link_to_program();
	ContextHelper::add_shader_to_hot_reload(m_shader_rope_body);

	m_shader_environment->add_shader(GL_VERTEX_SHADER, base_path, "shaders/environment_vs.glsl");
	m_shader_environment->add_shader(GL_FRAGMENT_SHADER, base_path, "shaders/environment_fs.glsl");
	m_shader_environment->compile_and_link_to_program();
	ContextHelper::add_shader_to_hot_reload(m_shader_environment);

	m_shader_lava->add_shader(GL_VERTEX_SHADER, base_path, "shaders/lava_vs.glsl");
	m_shader_lava->add_shader(GL_FRAGMENT_SHADER, base_path, "shaders/lava_fs.glsl");
	m_shader_lava->compile_and_link_to_program();
	ContextHelper::add_shader_to_hot_reload(m_shader_lava);

	m_shader_platforms->add_shader(GL_VERTEX_SHADER, base_path, "shaders/platforms_vs.glsl");
	m_shader_platforms->add_shader(GL_FRAGMENT_SHADER, base_path, "shaders/platforms_fs.glsl");
	m_shader_platforms->compile_and_link_to_program();
	ContextHelper::add_shader_to_hot_reload(m_shader_platforms);

}

void Game::init_game()
{
	m_player.init();

	m_size_of_rope = m_control_point_per_rope_count * sizeof(vec4);
	m_rope_bank_pos_VBO.re_allocate(m_rope_bank_size * m_size_of_rope);// 1 VBO for all ropes position
	m_rope_bank_tan_VBO.re_allocate(m_rope_bank_size * m_size_of_rope);// 1 VBO for all ropes tangents
	
	//Rebuild VAO:
	m_vao_ropes.set_channel_float_type(0, m_rope_bank_pos_VBO.m_buffer_id, 4);//positions in channel 0 in rope shader
	m_vao_ropes.set_channel_float_type(1, m_rope_bank_tan_VBO.m_buffer_id, 4);//tangents in channel 1 in rope shader

	m_lava_altitude = 0.0f;
	m_time_game_s = 0.0f;

	last_rope_thrown = m_rope_bank_size -1;
}

void Game::write_ropes_data_to_vbos()
{
	for (int i = 0; i < m_rope_bank_size; i++)
	{
		m_rope_bank_pos_VBO.write_to_gpu(m_rope_bank[i].p_shading, i * m_size_of_rope, m_size_of_rope);
		m_rope_bank_tan_VBO.write_to_gpu(m_rope_bank[i].t_shading, i * m_size_of_rope, m_size_of_rope);
	}
}

void Game::ropes_physics()
{
	for (int i = 0; i < m_rope_bank_size; i++)
	{
		m_rope_bank[i].compute_rope_physics(ContextHelper::time_frame_s, m_gravity_amplitude);
	}
}

void Game::throw_rope()
{
	for (int i = 0; i < m_rope_bank_size; i++)
	{
		if (!m_rope_bank[last_rope_thrown].is_left_hand)
		{
			m_rope_bank[i].is_tail_bound_to_player = false;
		}
	}
	last_rope_thrown = (last_rope_thrown + 1) % m_rope_bank_size;
	for (int i = 0; i < m_control_point_per_rope_count; i++)
	{
		m_rope_bank[last_rope_thrown].p[i] = m_player.p + vec3(0.0f, 1.0f - i / 10.0f, 0.0f);//to change
		m_rope_bank[last_rope_thrown].v[i] = vec3(0.0f);
		m_rope_bank[last_rope_thrown].a[i] = vec3(0.0f);
	}
	m_rope_bank[last_rope_thrown].v[0] = m_player.m_forward * m_player.m_rope_throw_intensity;
	m_rope_bank[last_rope_thrown].is_head_bound_to_platform = false;
	m_rope_bank[last_rope_thrown].is_tail_bound_to_player = true;
	m_rope_bank[last_rope_thrown].is_left_hand = false;
	
}


void Game::write_params_to_application_struct(ApplicationUboDataStructure& app_ubo)
{
	/*will need to complete with camera start
	app_ubo.player_pos.x = m_player.p.x;
	app_ubo.player_pos.y = m_player.p.y;
	app_ubo.player_pos.z = m_player.p.z;
	app_ubo.player_pos.w = m_time_game_s;
	*/
	app_ubo.rope_color.w = 0.1f;
	app_ubo.lava_params.x = m_lava_altitude;
	app_ubo.platform_cell_length = m_platform_cell_length;
	app_ubo.platform_cell_offset = m_platform_cell_offset;
	app_ubo.platform_length = m_platform_length;
}




void Game::draw_lava()
{
	m_vao_dummy.use_vao();
	m_shader_lava->use_shader_program();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);//2 triangle to draw a quad
	glFlush();
}

void Game::draw_platforms()
{
	m_vao_dummy.use_vao();
	m_shader_platforms->use_shader_program();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);//2 triangle to draw a quad covering screen
	glFlush();
}

void Game::draw_environment()
{
	m_vao_dummy.use_vao();
	m_shader_environment->use_shader_program();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);//2 triangle to draw a quad covering screen
	glFlush();
}

void Game::draw_ropes()
{
	m_vao_ropes.use_vao();
	m_shader_rope_body->use_shader_program();
	for (int i = 0; i < m_rope_bank_size; i++)
	{
		glDrawArrays(GL_LINE_STRIP, m_control_point_per_rope_count*i, m_control_point_per_rope_count);
		glFlush();
	}
}


void Game::gui(ApplicationUboDataStructure& app_ubo)
{
	if (ImGui::Button("Start Game"))
	{
		init_game();
		m_game_status = 1;//Game started
	}
	ImGui::ColorEdit3("Rope color", &(app_ubo.rope_color.x));
	ImGui::SliderFloat("Rope radius", &(app_ubo.rope_color.w), 0.1f, 2.0f);
	m_player.gui();
	Rope::gui();
	//m_plateforms.gui();
	if (ImGui::TreeNode("Gameplay"))
	{
		ImGui::SliderFloat("Lava speed", &(m_lava_speed), 0.0f, 2.0f);
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Game physics "))
	{
		int rope_bank_size = m_rope_bank_size;
		ImGui::SliderInt("Rope bank size", &(rope_bank_size), 2, 200);
		m_rope_bank_size = rope_bank_size;

		int point_per_rope = m_control_point_per_rope_count;
		ImGui::SliderInt("Rope bank size", &(point_per_rope), 2, 200);
		m_control_point_per_rope_count = point_per_rope;

		ImGui::SliderFloat("Gravity amplitude", &(m_gravity_amplitude), 0.1f, 10.0f);
		ImGui::SliderFloat("Delta time (sec)", &(m_delta_t_s), 0.001f, 0.010f);

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Platforms "))
	{
		ImGui::SliderFloat("Cell length X", &(m_platform_cell_length.x), 50.0f, 500.0f);
		ImGui::SliderFloat("Cell length Y", &(m_platform_cell_length.y), 50.0f, 500.0f);
		ImGui::SliderFloat("Cell length Z", &(m_platform_cell_length.z), 50.0f, 500.0f);
		ImGui::SliderFloat("Empty likelihood", &(m_platform_cell_length.w), 0.0f, 1.0f);

		ImGui::SliderFloat("Offset cell length X", &(m_platform_cell_offset.x), 5.0f, 20.0f);
		ImGui::SliderFloat("Offset cell length Y", &(m_platform_cell_offset.y), 5.0f, 20.0f);
		ImGui::SliderFloat("Offset cell length Z", &(m_platform_cell_offset.z), 5.0f, 20.0f);

		ImGui::SliderFloat("Min length", &(m_platform_length.x), 0.0f, 200.0f);
		ImGui::SliderFloat("Max length", &(m_platform_length.y), 0.0f, 200.0f);
		ImGui::SliderFloat("Min width", &(m_platform_length.z), 0.0f, 200.0f);
		ImGui::SliderFloat("Max width", &(m_platform_length.w), 0.0f, 200.0f);
		ImGui::TreePop();
	}
}