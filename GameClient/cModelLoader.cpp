#include "cModelLoader.h"

#include <iostream>			// cin cout
#include <fstream>		    // ifstream ofstream

cModelLoader::cModelLoader()			// constructor
{
	//std::cout << "A cModelLoader() is created!" << std::endl;
	return;
}

cModelLoader::~cModelLoader()			// destructor
{
	//std::cout << "A cModelLoader() is destroyed. How sad." << std::endl;
	return;
}

// Takes the filename to load
// Returns by ref the mesh
bool cModelLoader::LoadPlyModel(
	std::string filename,
	cMesh &theMesh)				// Note the "&"
{
	std::string AABB_file = "assets/models/Terrain_XYZ_n_uv.ply";
	//std::string AABB_file = "assets/models/Halo_Ring2_XYZ_N_UV.ply";

	std::ifstream theFile( filename.c_str() );
	if ( ! theFile.is_open() )
	{	
		// On no! Where's the file??? 
		return false;
	}

	// Scan the file until I get to "vertex", and stop
	std::string temp;
	//theFile >> temp;
	while ( theFile >> temp ) 
	{
		// Have it hit the word "vertex"?
		if ( temp == "vertex" )
		{
			break;		// Exit the while loop
		}
	}// while ( theFile >> temp ) 
	
	unsigned int numberOfVertices;
	theFile >> numberOfVertices;



	while (theFile >> temp)
	{
		// Have it hit the word "face"?
		if (temp == "face")
		{
			break;		// Exit the while loop
		}
	}// while ( theFile >> temp ) 

	unsigned int numberOfTriangles;
	theFile >> numberOfTriangles;

	while (theFile >> temp)
	{
		// Have it hit the word "end_header"?
		if (temp == "end_header")
		{
			break;		// Exit the while loop
		}
	}// while ( theFile >> temp ) 



	// Read all the vertices
	for (unsigned int index = 0; index != numberOfVertices; index++)
	{
		// -0.0312216 0.126304 0.00514924
		sPlyVertexXYZ tempVertex;

		theFile >> tempVertex.x >> tempVertex.y >> tempVertex.z;
		theFile >> tempVertex.nx >> tempVertex.ny >> tempVertex.nz;
		theFile >> tempVertex.u >> tempVertex.v;

		if (filename == AABB_file)
		{
			if (tempVertex.x < theMesh.min.x) theMesh.min.x = tempVertex.x;
			if (tempVertex.y < theMesh.min.y) theMesh.min.y = tempVertex.y;
			if (tempVertex.z < theMesh.min.z) theMesh.min.z = tempVertex.z;

			if (tempVertex.x > theMesh.max.x) theMesh.max.x = tempVertex.x;
			if (tempVertex.y > theMesh.max.y) theMesh.max.y = tempVertex.y;
			if (tempVertex.z > theMesh.max.z) theMesh.max.z = tempVertex.z;
		}

		// Add this temp vertex to the vector of vertices
		// (cMesh &theMesh)
		theMesh.vecVertices.push_back( tempVertex );
	}

	for (unsigned int index = 0; index != numberOfTriangles; index++)
	{
		// 3 166 210 265 
		int discardThis;
		sPlyTriangle tempTriangle;

		theFile >> discardThis 
			>> tempTriangle.vert_index_1 
			>> tempTriangle.vert_index_2
			>> tempTriangle.vert_index_3;

		// Add this triangle
		theMesh.vecTriangles.push_back(tempTriangle);

		// calculate and cache mesh triangle
		sMeshTriangle tempMeshTriangle;
		sPlyVertexXYZ first = theMesh.vecVertices[tempTriangle.vert_index_1];
		sPlyVertexXYZ second = theMesh.vecVertices[tempTriangle.vert_index_2];
		sPlyVertexXYZ third = theMesh.vecVertices[tempTriangle.vert_index_3];
		tempMeshTriangle.first = glm::vec3(first.x, first.y, first.z);
		tempMeshTriangle.second = glm::vec3(second.x, second.y, second.z);
		tempMeshTriangle.third = glm::vec3(third.x, third.y, third.z);
		tempMeshTriangle.normal.x = (first.nx + second.nx + third.nx) / 3.f;
		tempMeshTriangle.normal.y = (first.ny + second.ny + third.ny) / 3.f;
		tempMeshTriangle.normal.z = (first.nz + second.nz + third.nz) / 3.f;

		tempMeshTriangle.m1 = (tempMeshTriangle.first + tempMeshTriangle.second) / 2.f;
		tempMeshTriangle.m2 = (tempMeshTriangle.first + tempMeshTriangle.third) / 2.f;
		tempMeshTriangle.m3 = (tempMeshTriangle.second + tempMeshTriangle.second) / 2.f;
		tempMeshTriangle.m1 = (tempMeshTriangle.m1 + tempMeshTriangle.m2 + tempMeshTriangle.m3) / 3.f;

		theMesh.vecMeshTriangles.push_back(tempMeshTriangle);
	}

	if (filename == AABB_file)
	{
		if (theMesh.min.x < min.x) min.x = theMesh.min.x;
		if (theMesh.min.y < min.y) min.y = theMesh.min.y;
		if (theMesh.min.z < min.z) min.z = theMesh.min.z;

		if (theMesh.max.x > max.x) max.x = theMesh.max.x;
		if (theMesh.max.y > max.y) max.y = theMesh.max.y;
		if (theMesh.max.z > max.z) max.z = theMesh.max.z;
	}

	return true;
}
