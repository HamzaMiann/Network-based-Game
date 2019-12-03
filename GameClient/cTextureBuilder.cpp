#include "cTextureBuilder.h"
#include "TextureManager/cBasicTextureManager.h"
#include <iostream>
using namespace rapidxml;

void cTextureBuilder::Build(Scene& scene, xml_node<>* node)
{
	printf("Loading Textures...\n");

	cBasicTextureManager::Instance()->SetBasePath("assets/textures");
	for (xml_node<>* tex_node = node->first_node("Texture"); tex_node; tex_node = tex_node->next_sibling("Texture"))
	{
		std::string texture = tex_node->first_attribute("name")->value();
		if (!cBasicTextureManager::Instance()->Create2DTextureFromBMPFile(texture, true))
		{
			std::cout << "Didn't load texture" << std::endl;
		}
		
	}
}
