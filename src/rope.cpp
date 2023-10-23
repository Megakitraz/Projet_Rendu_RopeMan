#include "rope.hpp"
#include "player.hpp"

Rope::Rope(std::uint32_t control_point_count, Player& player) : m_player(player)
{
	m_control_point_count = control_point_count;
	p_shading = new vec4[m_control_point_count];
	t_shading = new vec4[m_control_point_count];
	p = new vec3[m_control_point_count];
	v = new vec3[m_control_point_count];
	a = new vec3[m_control_point_count];

	for (int i = 0; i < m_control_point_count; i++)
	{
		p[i] = vec3(i / 10.0f, i / 10.0f, i * i / 100.0f);//to change
		v[i] = vec3(0.0f);
		a[i] = vec3(0.0f);
	}
	is_head_bound_to_platform = false;
	is_tail_bound_to_player = false;

	compute_p_shading();
}



void Rope::compute_p_shading()
{
	for (int i = 0; i < m_control_point_count; i++)
	{

		vec3 shift;
		if (is_left_hand)
			shift = m_player.m_local_left * m_player.m_radius;
		else
			shift = -m_player.m_local_left * m_player.m_radius;

		shift *= float(i) / float(m_control_point_count - 1);

		p_shading[i].x = p[i].x + shift.x;
		p_shading[i].y = p[i].y + shift.y;
		p_shading[i].z = p[i].z + shift.z;
		p_shading[i].w = float(is_left_hand);
	}
}


/*
Rope::~Rope()
{
	delete[] p_shading;
	delete[] t_shading;
	delete[] p;
	delete[] v;
	delete[] a;
}*/

void Rope::gui()
{
	//ImGui::ColorEdit3(("Player " + std::to_string(m_id)).c_str(), &(m_color.x));
}