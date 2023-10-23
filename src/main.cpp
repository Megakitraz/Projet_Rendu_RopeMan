#define GLFW_INCLUDE_NONE
#define TINYOBJLOADER_C_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define GLM_FORCE_LEFT_HANDED // because of OpenGL NDC
#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <thread>
#include <tiny_obj_loader_c.h>
#include <stb_image_write.h>
#include "helpers/helpers_common.hpp"
#include "buffer_structures.hpp"

#include "player.hpp"
#include "game.hpp"

#define FOLDER_ROOT "../"


int main(int argc, char* argv[]) {

	ContextHelper::init_context_all(1440, 900, "Rope Man",8);
	ContextHelper::print_opengl_info();

	//Projection matrix
	ProjectionMatrix proj;
	proj.set_viewport_resolution(ContextHelper::resolution);
	proj.set_perspective(70.0f, 0.1f, 5000.0f);//maybe to adjust to scene
	//WorldView matrix
	FreeFlyCamera cam; // Maybe this class will be modified to have a "walk" mode (forced just above the ground)
	cam.set_camera(vec3(-35.0f,30.0f,-30.0f),45.0f,-20.0f);
	cam.set_params(0.1f,10.0f,50.0f);


	Game game;
	game.load_shaders(FOLDER_ROOT);
	game.load_cubemap(FOLDER_ROOT,"cubemap_snow");




	//UBO init
	ApplicationUboDataStructure app_ubo_data;
	GPUBuffer application_ubo;
	application_ubo.allocate(sizeof(ApplicationUboDataStructure));
	application_ubo.set_target_and_slot(GL_UNIFORM_BUFFER, UBO_APPLICATION_BINDING);


	//OpenglFlags
	glClearColor(0.0f,0.0f, 0.0f,1.0f); // background is black
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_BACK);
	//glFrontFace(GL_CCW);;
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	while (ContextHelper::should_not_close_window()) //main/render loop
	{
		ContextHelper::begin_frame();
		if (ContextHelper::window_resized)
		{
			glViewport(0, 0, ContextHelper::resolution.x, ContextHelper::resolution.y);
		}
		proj.set_viewport_resolution(ContextHelper::resolution);
		cam.flush();

		//Flush Application UBO
		//will be deleted and replaced by player matrices 
		app_ubo_data.proj = proj.m_proj;
		app_ubo_data.inv_proj = glm::inverse(proj.m_proj);
		app_ubo_data.w_v = cam.m_w_v;
		app_ubo_data.w_v_p = proj.m_proj * cam.m_w_v;
		app_ubo_data.inv_w_v_p = glm::inverse(app_ubo_data.w_v_p);
		app_ubo_data.player_pos.x = cam.m_pos.x;
		app_ubo_data.player_pos.y = cam.m_pos.y;
		app_ubo_data.player_pos.z = cam.m_pos.z;
		app_ubo_data.lava_params.x = 1.0f;
		//
		game.write_params_to_application_struct(app_ubo_data);
		application_ubo.write_to_gpu(&app_ubo_data);

		game.write_ropes_data_to_vbos();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//clear Frambuffer channel + Z-buffer 
		game.draw_platforms();
		game.draw_lava();
		game.draw_ropes();
		game.draw_environment();
		glFinish();//Force wait for GPU to finish jobs, since the post_process shader will read from rendered textures


		//ImGui interface starts here
		ImGui::Begin("Parameters");

		if (ImGui::TreeNode("General"))
		{
			static float font_scale = 1.0f;
			ImGui::SetWindowFontScale(font_scale);
			ImGui::SliderFloat("text scale", &font_scale, 0.5f, 4.0f);
			
			ImGui::Text(("Camera position: " + std::to_string(cam.m_pos.x) + " " + std::to_string(cam.m_pos.y) + " " + std::to_string(cam.m_pos.z) + " ").c_str());
			ImGui::Text(("Camera direction: " + std::to_string(cam.m_w_v[0].z) + " " + std::to_string(cam.m_w_v[1].z) + " " + std::to_string(cam.m_w_v[2].z) + " ").c_str());
			ImGui::TreePop();
		}
		game.gui(app_ubo_data);
		ImGui::End();

		ContextHelper::end_frame();//glfwSwapBuffers [hookpoint for profiler/debugger]
	}
	ContextHelper::destroy_context_all();

	return EXIT_SUCCESS;
}
