

#include "cLightBuilder.h"
#include "cLightManager.h"
#include <glm/vec4.hpp>
using namespace rapidxml;


void _setXYZW(glm::vec4& vec4, xml_node<>* node)
{
	vec4.x = strtof(node->first_attribute("x")->value(), 0);
	vec4.y = strtof(node->first_attribute("y")->value(), 0);
	vec4.z = strtof(node->first_attribute("z")->value(), 0);
	vec4.w = strtof(node->first_attribute("w")->value(), 0);
}

void cLightBuilder::Build(Scene& scene, xml_node<>* node)
{
	printf("Building Lights...\n");

	scene.pLightManager = new cLightManager();
	for (xml_node<>* light_node = node->first_node("Light"); light_node; light_node = light_node->next_sibling("Light"))
	{
		sLight* light = new sLight();
		for (xml_node<>* attr = light_node->first_node(); attr; attr = attr->next_sibling())
		{
			std::string name = attr->name();
			if (name == "Position")
			{
				_setXYZW(light->position, attr);
			}
			else if (name == "Diffuse")
			{
				_setXYZW(light->diffuse, attr);
			}
			else if (name == "Specular")
			{
				_setXYZW(light->specular, attr);
			}
			else if (name == "Atten")
			{
				_setXYZW(light->atten, attr);
			}
			else if (name == "Direction")
			{
				_setXYZW(light->direction, attr);
			}
			else if (name == "Param1")
			{
				_setXYZW(light->param1, attr);
			}
			else if (name == "Param2")
			{
				_setXYZW(light->param2, attr);
			}
		}
		scene.pLightManager->Lights.push_back(light);
	}
}