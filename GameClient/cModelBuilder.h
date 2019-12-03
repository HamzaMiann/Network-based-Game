#pragma once


#include "iSceneBuilder.h"

class cModelBuilder : public iSceneBuilder
{
public:
	cModelBuilder() {}
	virtual ~cModelBuilder() {}
	virtual void Build(Scene& scene, rapidxml::xml_node<>* node);
};