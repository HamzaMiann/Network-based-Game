

#include "cModelBuilder.h"
#include "cVAOManager.h"
#include "cModelLoader.h"
#include <iostream>
using namespace rapidxml;

void cModelBuilder::Build(Scene& scene, xml_node<>* node)
{
	printf("Loading models...\n");

	scene.pVAOManager = cVAOManager::Instance();
	scene.pModelLoader = cModelLoader::Instance();

	for (xml_node<>* model_node = node->first_node("Model"); model_node; model_node = model_node->next_sibling("Model"))
	{
		xml_attribute<>* file = model_node->first_attribute("file");
		xml_attribute<>* friendly = model_node->first_attribute("friendlyName");
		xml_attribute<>* shader = model_node->first_attribute("shader");
		if (file)
		{
			std::string fileName = file->value();
			std::string friendlyName = "";

			if (friendly)
				friendlyName = friendly->value();

			cMesh* pMesh = new cMesh();
			if (scene.pModelLoader->LoadPlyModel("assets/models/" + fileName, *pMesh))
			{

				sModelDrawInfo* pDrawInfo = new sModelDrawInfo();
				scene.pVAOManager->LoadModelIntoVAO(friendlyName,
													*pMesh,
													*pDrawInfo,
													scene.Shaders[shader->value()]);
			}
			else
			{
				std::cout << "Model builder could not find/load file: " << fileName << std::endl;
			}
		
		}
	}
}