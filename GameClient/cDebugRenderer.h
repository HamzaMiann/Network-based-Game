#pragma once
#ifndef _cDebugRenderer_HG_
#define _cDebugRenderer_HG_

// This is a complete debug thing for rendering wireframe lines
#include "iDebugRenderer.h"
#include <glm/glm.hpp>
#include <string>
#include <vector>	

// Note: 
// - Include this header in the thing(s) that MANAGE the debug render
//   (init, rendering, etc.)
// - Include the ++iDebugRenderer++ in the things that need to add things to draw
//

class cGLRenderStateHelper;	// Forward declare so we don't have to include the GL headers

class cDebugRenderer : public iDebugRenderer
{
private:
	cDebugRenderer();

public:
	~cDebugRenderer();

	static cDebugRenderer* Instance()
	{
		static cDebugRenderer instance;
		return &instance;
	}

	// Default maximum number of objects in buffer before a resize is forced
	static const unsigned int DEFAULTNUMBEROFTRIANGLES = 1000;
	static const unsigned int DEFAULTNUMBEROFLINES = 1000;
	static const unsigned int DEFAULTNUMBEROFPOINTS = 1000;
	// This is how much bigger the mesh buffer is, in comparison to the 
	//	acutal size of all the meshes. Eliminates the need for byte alignment, 
	//	at the cost of extra space used at the end of the buffer
	static const float DEFAULT_MESHBUFFERSIZE_PADDING_RATIO;	// = 1.10 or 10 %

	bool initialize(void);
	bool IsOK(void);

	bool resizeBufferForTriangles(unsigned int newNumberOfTriangles);
	bool resizeBufferForLines(unsigned int newNumberOfLines);
	bool resizeBufferForPoints(unsigned int newNumberOfPoints);
	// This takes into account the number of meshes and adds some padding
	bool resizeBufferForMeshes(float meshBufferPaddingRatio = DEFAULT_MESHBUFFERSIZE_PADDING_RATIO);

	std::string getLastError( bool bClearError = true );


	// Renders scene
	void RenderDebugObjects(glm::mat4 matCameraView, glm::mat4 matProjection, double deltaTime);
private:
	void m_RenderDebugTriangles(glm::mat4 matCameraView, glm::mat4 matProjection, double deltaTime);
	void m_RenderDebugLines(glm::mat4 matCameraView, glm::mat4 matProjection, double deltaTime);
	void m_RenderDebugPoints(glm::mat4 matCameraView, glm::mat4 matProjection, double deltaTime);
	void m_RenderDebugMeshes(glm::mat4 matCameraView, glm::mat4 matProjection, double deltaTime);

	// Used to save and restore the render state during the debug render calls
	cGLRenderStateHelper* m_pGLRenderState;


	// Used to compile the shader
	void m_parseStringIntoMultiLine( std::string singleLineSource, std::vector<std::string> &vecMultiline );
	bool m_compileShaderFromSource( unsigned int &shaderID, std::vector<std::string> &vecMultilineSource, std::string &error );
	bool m_wasThereALinkError( unsigned int programID, std::string &error );

public:

	static const float DEFAULT_POINT_SIZE;	// = 1.0f;

	virtual void addTriangle(glm::vec3 v1XYZ, glm::vec3 v2XYZ, glm::vec3 v3XYZ, glm::vec3 colour, float lifeTime=0.0f);
	virtual void addTriangle(drTri &tri);
	virtual void addLine(glm::vec3 startXYZ, glm::vec3 endXYZ, glm::vec3 colour, float lifeTime=0.0f);
	virtual void addLine(drLine &line);
	virtual void addPoint(glm::vec3 xyz, glm::vec3 colour, float lifeTime=0.0f, float pointSize=1.0f);
	virtual void addPoint(drPoint &point);

	// Replaces the DrawDebugSphere
	virtual void addDebugSphere(glm::vec3 xyz, glm::vec3 colour, float scale, float lifeTime=0.0f);
	virtual void addDebugMesh(std::string meshName, glm::vec3 xyz, glm::vec3 orientXYZ_Euler, glm::vec3 colour, float scale, float lifeTime=0.0f);
	virtual void addDebugMesh(std::string meshName, glm::vec3 xyz, glm::quat orientXYZ_Quaternion, glm::vec3 colour, float scale, float lifeTime=0.0f);

	// Various meshes that you could load and draw (are drawn with static meshes)
	// Note: the mesh is ONLY triangles, so NOT indexed (like ply, obj, etc.)
	virtual void loadDebugMesh(std::string friendlyName, std::vector<sDebugTri> &vecTris);
	// This is used to load the hard coded sphere
	virtual void generateDefaultSphereMesh(std::vector<sDebugTri> &vecTris );
	// Generates a sphere, and returns the triangles in the vector of triangles
	virtual void generateIsoSphere( unsigned int numIterations, float radius, std::vector<sDebugTri> &vecTris);
	//virtual void generateSphereAnnealing(sAnnealingSphereSettings settings, float radius, std::vector<sDebugTri> &vecTris);


public:
	unsigned int getTriangleBufferSizeInTriangles(void)	{ return this->m_VAOBufferInfoTriangles.bufferSizeObjects; }
	unsigned int getTriangleBufferSizeInBytes(void)		{ return this->m_VAOBufferInfoTriangles.bufferSizeBytes; }
	unsigned int getLineBufferSizeInLines(void)			{ return this->m_VAOBufferInfoLines.bufferSizeObjects; }
	unsigned int getLineBufferSizeInBytes(void)			{ return this->m_VAOBufferInfoLines.bufferSizeBytes; }
	unsigned int getPointBufferSizeInPoints(void)		{ return this->m_VAOBufferInfoPoints.bufferSizeObjects; }
	unsigned int getPointBufferSizeInBytes(void)		{ return this->m_VAOBufferInfoPoints.bufferSizeBytes; }

	bool getShadersUsed(std::string &vertexShaderSource, std::string &fragmentShaderSource);
	bool setVertexShader(std::string vertexShaderSource);
	bool setFragmentShader(std::string fragmentShaderSource);
	bool setShaders(std::string vertexShaderSource, std::string fragmentShaderSource);

	// Quick-n-Dirty utility to convert ply format to "flat" (only triangle) format
	bool QnD_convertIndexedXYZPlyToTriangleOnlyVertices( const std::string &plyText, std::vector<sDebugTri> &vecTris );
	bool QnD_convertIndexedXYZPlyFileToTriangleOnlyVertices( std::string fileName, std::vector<sDebugTri> &vecTris );
	// This converts a vector of sDebugTris into a header file representation
	// "arrayName" is the name of the array and the size value:
	// - arrayName+"_array" for the array		(example: "float teapot_array[] = ...")
	// - arrayName+"_array_size" for the size	(example: "unsigned int teapot_array_size = ...")
	// NOTE: Size is the TOTAL number of floats, so 3x the number of triangles. 
	//       You pass this TOTAL number info the loadHeaderArrayInto_vecTri() method.
	// Leave outputFileName blank to NOT save to a file.
	bool QnD_convert_vecTri_to_array_header( std::vector<sDebugTri> &vecTris, std::string arrayName, std::string &arrayText, std::string outputFileName = "" );
	bool QnD_loadHeaderArrayInto_vecTri( float* shapeArray, int sizeOfArray, std::vector<sDebugTri> &vecTris );
	static const std::string DEFAULT_PLY_SPHERE_MODEL_TEXT;	
private:

	unsigned int m_RoundUpToNearest100( unsigned int value )
	{
		return (value + 100) / 100;
	}

	// Vertex and Fragment are same for both
	std::string m_vertexShaderSource;
	std::string m_fragmentShaderSource;

	std::string m_geometryShaderSource_Triangles;
	std::string m_geometryShaderSource_Lines;

	// As objects are added (to draw), they are added to these containers
	std::vector<drTri>   m_vecTriangles;	
	std::vector<drLine>  m_vecLines;		
	std::vector<drPoint> m_vecPoints;	
	std::vector<drMesh>  m_vecMeshes;	

	static const std::string DEFALUT_VERT_SHADER_SOURCE;
	static const std::string DEFAULT_FRAG_SHADER_SOURCE;
	static const std::string DEFAULT_GEOM_SHADER_SOURCE_LINES;
	static const std::string DEFAULT_GEOM_SHADER_SOURCE_TRIANGLES;

	// This is the point that's inside the vertex buffer
	struct sVertex_xyzw_rgba
	{
		sVertex_xyzw_rgba() :
			x(0.0f), y(0.0f), z(0.0f), w(1.0f),
			r(0.0f), g(0.0f), b(0.0f), a(1.0f)
		{};
		float x, y, z, w;
		float r, g, b, a;
	};
	struct sVAOInfoDebug
	{
		sVAOInfoDebug() :
			shaderID(0),
			VAO_ID(0),
			vertex_buffer_ID(0),
			numberOfVerticesToDraw(0),
			numberOfObjectsToDraw(0),
			bufferSizeBytes(0),
			bufferSizeObjects(0),
			bufferSizeVertices(0),
			pLocalVertexArray(0),
			bIsValid(false)
		{ };
		unsigned int shaderID;				// needed to get the uniforms for the VAO
		unsigned int VAO_ID;	
		unsigned int vertex_buffer_ID;		// GLuint
		// These change each frame
		unsigned int numberOfVerticesToDraw;
		unsigned int numberOfObjectsToDraw;	// points, lines, triangles
		// These are constant, the max size of buffer
		unsigned int bufferSizeBytes;
		unsigned int bufferSizeObjects;
		unsigned int bufferSizeVertices;
		sVertex_xyzw_rgba* pLocalVertexArray;
		bool bIsValid;	// used to see if this has been set up
	};

	sVAOInfoDebug m_VAOBufferInfoTriangles;
	sVAOInfoDebug m_VAOBufferInfoLines;
	sVAOInfoDebug m_VAOBufferInfoPoints;
	// This contains all meshes, indexed by name
	std::map<std::string /*mesh name*/, sVAOInfoDebug> m_VAOBufferInfoMeshes;

	// These are used to locate a debug MESH inside the mesh vertex buffer
	struct sMeshInfo
	{
		std::string friendlyName;
		int firstVertexIndex;		// GLint first,
 		int numberOfIndices;		// GLsizei count
		// This is kept around if the vertex buffer has to be resized
		std::vector<sDebugTri> *p_vecTris;
	};
	std::map< std::string /*friendlyName*/, sMeshInfo > m_mapDebugMeshesLoaded;

	// This is used to create all the buffers
	bool m_InitBuffer(sVAOInfoDebug &VAOInfo);

	// Copies the debug objects from the vectors to the vertex buffer to render
	// Will delete any "non persistent" object
	void m_copyTrianglesIntoRenderBuffer(double deltaTime);
	void m_copyLinesIntoRenderBuffer(double deltaTime);
	void m_copyPointsIntoRenderBuffer(double deltaTime);

	std::map<std::string /*meshname*/, sVAOInfoDebug /*drawInfo*/> m_mapMeshNameToVAOInfo;

	// Helper functions for the annealing sphere thing


	// Used to hold the shader information
	// Note: it's set up this way so that we don't have to include the shader manager 
	//	in this file or set up a pimpl, etc. The cShaderProgramInfo is defined in
	//	the implementation file for the cDebugRenderer.cpp
//	class cShaderProgramInfo;
//	cShaderProgramInfo* m_pShaderProg_Lines;
//	cShaderProgramInfo* m_pShaderProg_Triangles;

	unsigned int m_LineShaderProgramID;
	int m_LineShader_matModelUniformLoc;
	int m_LineShader_matViewUniformLoc;
	int m_LineShader_matProjectionUniformLoc;

	unsigned int m_TriangleShaderProgramID;
	int m_TriangleShader_matModelUniformLoc;
	int m_TriangleShader_matViewUniformLoc;
	int m_TriangleShader_matProjectionUniformLoc;

	std::string m_lastError;

	template <class T>
	T randInRange( T min, T max )
	{
		double value = 
			min + static_cast <double> (rand()) 
			/ ( static_cast <double> (RAND_MAX/(static_cast<double>(max-min) )));
		return static_cast<T>(value);
	};
private:

};

#endif
