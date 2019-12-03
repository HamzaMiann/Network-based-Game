#pragma once

#include <vector>
#include "sLight.h"

class cLightManager
{
public:
	std::vector<sLight*> Lights;
	void Set_Light_Data(int shader_id);
};