#ifndef PLATEFORM_CLASS
#define PLATEFORM_CLASS

#include"buffer_structures.hpp"
#include "helpers/helpers_common.hpp"
#include <GLFW/glfw3.h>

class Plateform {

public:
	Plateform(float l, float w, float h, vec3 p);
	//~Plateform();
	void init();


private:
	float length;
	float width;
	float height;
	vec3* pos;
};

#endif