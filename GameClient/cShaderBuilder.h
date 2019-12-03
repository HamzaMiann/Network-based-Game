#pragma once


#include "iSceneBuilder.h"

class cShaderBuilder : public iSceneBuilder
{
public:
	cShaderBuilder() {}
	virtual ~cShaderBuilder() {}
	virtual void Build(Scene& scene, rapidxml::xml_node<>* node);
};