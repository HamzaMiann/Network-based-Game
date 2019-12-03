#pragma once


#include "iSceneBuilder.h"
#include <string>

class cSceneFactory
{
public:
	iSceneBuilder* GetBuilder(std::string builder_name);
};