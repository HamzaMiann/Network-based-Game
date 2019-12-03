#include "cGLRenderStateHelper.h"
#include <sstream>

cGLRenderStateHelper::cGLRenderStateHelper()
{
	return;
}


void cGLRenderStateHelper::SaveCurrentGLState(void)
{
	glGetBooleanv( GL_DEPTH_TEST, &(this->GL_depth_test_state) );				// glEnable(GL_DEPTH_TEST);
	glGetIntegerv( GL_POLYGON_MODE, &(this->GL_polygon_mode_state) );			// glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glGetBooleanv( GL_CULL_FACE, &(this->GL_cull_face_enabled_state) );			// glEnable(GL_CULL_FACE);
	glGetIntegerv( GL_CULL_FACE_MODE, &(this->GL_cull_face_mode_state) );			// glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

	return;
}

bool cGLRenderStateHelper::RestoreCurrentGLState(std::string &error)
{
	std::stringstream ssError;

	switch (this->GL_depth_test_state)
	{
	case GL_TRUE:
		break;
	case GL_FALSE:
		break;
	default:	// Error
		ssError << "Unexpected GL_DEPTH_TEST state: " << this->GL_depth_test_state << std::endl;
		break;
	}//switch (this->GL_depth_test_state)

	switch (this->GL_polygon_mode_state)
	{
	case GL_POINT:
		break;
	case GL_LINE:
		break;
	case GL_FILL:
		break;
	default:	// Error
		ssError << "Unexpected glPolygonMode() state: " << this->GL_polygon_mode_state << std::endl;
		break;
	}//switch (this->GL_polygon_mode_state)

	switch (this->GL_cull_face_enabled_state)
	{
	case GL_TRUE:
		break;
	case GL_FALSE:
		break;
	default:	// Error
		ssError << "Unexpected GL_CULL_FACE state: " << this->GL_cull_face_enabled_state << std::endl;
		break;
	}//switch (this->GL_cull_face_enabled_state)


	switch (this->GL_cull_face_mode_state)
	{
	case GL_FRONT:
		break;
	case GL_BACK:
		break;
	case GL_FRONT_AND_BACK:
		break;
	default:	// Error
		ssError << "Unexpected glCullFace() GL_CULL_FACE_MODE state: " << this->GL_cull_face_mode_state << std::endl;
		break;
	}//switch (this->GL_cull_face_mode_state)



	return true;
}

