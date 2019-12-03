#pragma once


#include "iSceneBuilder.h"

class cLightBuilder : public iSceneBuilder
{
public:
	cLightBuilder() {}
	virtual ~cLightBuilder() {}
	virtual void Build(Scene& scene, rapidxml::xml_node<>* node);
};