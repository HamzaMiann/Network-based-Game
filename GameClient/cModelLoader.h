#ifndef _cModelLoader_HG_2019_09_09_
#define _cModelLoader_HG_2019_09_09_

#include <string>	
#include <vector>
#include "cMesh.h"

class cModelLoader
{
private:
	cModelLoader();			// constructor

public:

	static cModelLoader* Instance()
	{
		static cModelLoader instance;
		return &instance;
	}

	~cModelLoader();		// destructor

	glm::vec3 min = glm::vec3(FLT_MAX);
	glm::vec3 max = glm::vec3(FLT_MIN);

	// Takes the filename to load
	// Returns by ref the mesh
	bool LoadPlyModel(std::string filename,
		              cMesh &theMesh);		// Note the & (by reference)

};

#endif //_cModelLoader_HG_2019_09_09_

