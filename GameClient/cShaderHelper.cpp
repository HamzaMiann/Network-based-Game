#include "cShaderHelper.h"
#include "cShaderManager.h"
#include <iostream>

unsigned int cShaderHelper::BuildShaderProgram(std::string name, std::string vertexShaderFile, std::string fragShaderFile)
{
	cShaderManager* pTheShaderManager = cShaderManager::Instance();

	cShaderManager::cShader vertexShad;
	vertexShad.fileName = "assets/shaders/" + vertexShaderFile;

	cShaderManager::cShader fragShader;
	fragShader.fileName = "assets/shaders/" + fragShaderFile;

	if (!pTheShaderManager->createProgramFromFile(name, vertexShad, fragShader))
	{
		throw std::exception(("Shader could not be compiled - " + pTheShaderManager->getLastError()).c_str());
	}

	unsigned int toReturn = pTheShaderManager->getIDFromFriendlyName(name);

	return toReturn;
}
