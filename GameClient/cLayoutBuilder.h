#pragma once


#include "iSceneBuilder.h"

class cLayoutBuilder : public iSceneBuilder
{
public:
	cLayoutBuilder() {}
	virtual ~cLayoutBuilder() {}
	virtual void Build(Scene& scene, rapidxml::xml_node<>* node);
};