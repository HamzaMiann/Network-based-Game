#pragma once

#include "Scene.h"
#include <rapidxml/rapidxml.hpp>

class iSceneBuilder
{
public:
	virtual ~iSceneBuilder() {}
	virtual void Build(Scene& scene, rapidxml::xml_node<>* node) = 0;
};