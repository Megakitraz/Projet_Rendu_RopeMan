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
		p[i] = m_player.p + vec3(i / 100.0f, i / 100.0f, i * i / 10000.0f);//to change
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

void Rope::compute_tangents(){

}

void Rope::compute_rope_physics(float delta_time, float gravity){
	if(!is_head_bound_to_platform){

		p[0] = p[0] + delta_time*v[0];
		v[0] = v[0] + delta_time*a[0];
		a[0] = gravity*vec3(0,-1, 0);
	}
	for (int i = 1; i < m_control_point_count-1; i++){
		p[i] = p[i] + v[i]*delta_time;
		v[i] = v[i] + a[i]*delta_time;
		float former_segment_length = sqrt(dot(p[i]-p[i-1],p[i]-p[i-1]));
		float next_segment_length = sqrt(dot(p[i]-p[i+1],p[i]-p[i+1]));
		float former_segment_delta = former_segment_length - segment_length;
		float next_segment_delta = next_segment_length - segment_length;
		vec3 former_force_direction = normalize(p[i-1]-p[i]);
		vec3 next_force_direction = normalize(p[i+1]-p[i]);
		a[i] = gravity*vec3(0,-1, 0) 
		+ segment_k*former_segment_delta*former_force_direction/segment_mass 
		+ segment_k*next_segment_delta*next_force_direction/segment_mass;
	}
	p[m_control_point_count-1] = p[m_control_point_count-1] + v[m_control_point_count-1]*delta_time;
	v[m_control_point_count-1] = v[m_control_point_count-1] + a[m_control_point_count-1]*delta_time;
	float former_segment_length = sqrt(dot(p[m_control_point_count-1]-p[m_control_point_count-2],p[m_control_point_count-1]-p[m_control_point_count-2]));
	float former_segment_delta = former_segment_length - segment_length;
	vec3 former_force_direction = normalize(p[m_control_point_count-2]-p[m_control_point_count-1]);
	if(is_tail_bound_to_player){
		m_player.AddAcceleration(segment_k * former_segment_delta * former_force_direction / (segment_mass + m_player.m_mass));
		p[m_control_point_count - 1] = m_player.p;
		
	}
	else{
		a[m_control_point_count-1] = gravity*vec3(0,-1, 0) 
		+ segment_k*former_segment_delta*former_force_direction/(segment_mass);
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