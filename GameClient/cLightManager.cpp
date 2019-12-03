
#include "cLightManager.h"
#include "sLight.h"
#include "GLCommon.h"
#include <string>
#include <sstream>

void cLightManager::Set_Light_Data(int shader_id)
{
	GLint num_loc = glGetUniformLocation(shader_id, "NUMBEROFLIGHTS");
	glUniform1i(num_loc, (int)Lights.size());

	for (unsigned int i = 0; i < this->Lights.size(); ++i)
	{
		sLight* light = Lights[i];
		std::ostringstream oss;
		oss << "theLights[" << i << "]";
		std::string name = oss.str();
		GLint pos_loc = glGetUniformLocation(shader_id, (name + ".position").c_str());
		GLint diff_loc = glGetUniformLocation(shader_id, (name + ".diffuse").c_str());
		GLint spec_loc = glGetUniformLocation(shader_id, (name + ".specular").c_str());
		GLint atten_loc = glGetUniformLocation(shader_id, (name + ".atten").c_str());
		GLint direc_loc = glGetUniformLocation(shader_id, (name + ".direction").c_str());
		GLint param1_loc = glGetUniformLocation(shader_id, (name + ".param1").c_str());
		GLint param2_loc = glGetUniformLocation(shader_id, (name + ".param2").c_str());

		glUniform4f(pos_loc,
					light->position.x,
					light->position.y,
					light->position.z,
					light->position.w
		);

		glUniform4f(diff_loc,
					light->diffuse.x,
					light->diffuse.y,
					light->diffuse.z,
					light->diffuse.w
		);

		glUniform4f(spec_loc,
					light->specular.x,
					light->specular.y,
					light->specular.z,
					light->specular.w
		);

		glUniform4f(atten_loc,
					light->atten.x,
					light->atten.y,
					light->atten.z,
					light->atten.w
		);

		glUniform4f(direc_loc,
					light->direction.x,
					light->direction.y,
					light->direction.z,
					light->direction.w
		);

		glUniform4f(param1_loc,
					light->param1.x,
					light->param1.y,
					light->param1.z,
					light->param1.w
		);

		glUniform4f(param2_loc,
					light->param2.x,
					light->param2.y,
					light->param2.z,
					light->param2.w
		);
	}
}