#pragma once

#include "iSceneBuilder.h"

class cTextureBuilder : public iSceneBuilder
{
public:
	cTextureBuilder() {}
	virtual ~cTextureBuilder() {}
	virtual void Build(Scene& scene, rapidxml::xml_node<>* node);
};