
#include "cShaderBuilder.h"
#include "cShaderHelper.h"
#include <iostream>
using namespace rapidxml;

void cShaderBuilder::Build(Scene& scene, xml_node<>* node)
{
	printf("Compiling Shaders...\n");

	for (xml_node<>* shader_node = node->first_node("Shader"); shader_node; shader_node = shader_node->next_sibling("Shader"))
	{
		std::string vertShaderFile = shader_node->first_node("Vertex")->first_attribute("file")->value();
		std::string fragShaderFile = shader_node->first_node("Fragment")->first_attribute("file")->value();
		std::string name = shader_node->first_attribute("name")->value();
		try
		{
			scene.Shaders[name] = cShaderHelper::BuildShaderProgram(name, vertShaderFile, fragShaderFile);
		}
		catch (std::exception& err)
		{
			std::cout << "Error: " << err.what() << std::endl;
			exit(1);
		}
	}

}