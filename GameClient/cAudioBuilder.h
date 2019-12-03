#pragma once


#include "iSceneBuilder.h"

class cAudioBuilder : public iSceneBuilder
{
public:
	cAudioBuilder() {}
	virtual ~cAudioBuilder() {}
	virtual void Build(Scene& scene, rapidxml::xml_node<>* node);
};