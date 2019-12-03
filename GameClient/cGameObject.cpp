
#include "cGameObject.h"
#include <glm/gtc/matrix_transform.hpp>


cGameObject::cGameObject()
{
	this->textureRatio[0] = 1.f;
	this->textureRatio[1] = 0.f;
	this->textureRatio[2] = 0.f;
	this->textureRatio[3] = 0.f;
	this->texture[0] = "";
	this->texture[1] = "";
	this->texture[2] = "";
	this->texture[3] = "";

	this->force = glm::vec3(0.f);
	this->acceleration = glm::vec3(0.f);
	this->velocity = glm::vec3(0.f);
}

cGameObject::~cGameObject()
{
}

glm::mat4 cGameObject::ModelMatrix()
{
	return ((glm::mat4(1.f) * TranslationMatrix()) * RotationMatrix()) * ScaleMatrix();
}

glm::mat4 cGameObject::TranslationMatrix()
{
	return glm::translate(glm::mat4(1.0f),
						  glm::vec3(pos.x,
									pos.y,
									pos.z)
	);
}

glm::mat4 cGameObject::RotationMatrix()
{
	glm::mat4 m(1.f);
	m *= glm::mat4(getQOrientation());
	return m;
}

glm::mat4 cGameObject::ScaleMatrix()
{
	return glm::scale(glm::mat4(1.0f),
					  glm::vec3(scale,
								scale,
								scale)
	);
}


glm::quat cGameObject::getQOrientation(void)
{
	return this->rotation;
}

// Overwrite the orientation
void cGameObject::setOrientation(glm::vec3 EulerAngleDegreesXYZ)
{
	// c'tor of the glm quaternion converts Euler 
	//	to quaternion representation. 
	glm::vec3 EulerAngleRadians;
	EulerAngleRadians.x = glm::radians(EulerAngleDegreesXYZ.x);
	EulerAngleRadians.y = glm::radians(EulerAngleDegreesXYZ.y);
	EulerAngleRadians.z = glm::radians(EulerAngleDegreesXYZ.z);

	this->rotation = glm::quat(EulerAngleRadians);
}

void cGameObject::setOrientation(glm::quat qAngle)
{
	this->rotation = qAngle;
}

// Updates the existing angle
void cGameObject::updateOrientation(glm::vec3 EulerAngleDegreesXYZ)
{
	// Create a quaternion of this angle...
	glm::vec3 EulerAngleRadians;
	EulerAngleRadians.x = glm::radians(EulerAngleDegreesXYZ.x);
	EulerAngleRadians.y = glm::radians(EulerAngleDegreesXYZ.y);
	EulerAngleRadians.z = glm::radians(EulerAngleDegreesXYZ.z);

	glm::quat angleChange = glm::quat(EulerAngleRadians);

	// ...apply it to the exiting rotation
	this->rotation *= angleChange;
}

void cGameObject::updateOrientation(glm::quat qAngleChange)
{
	this->rotation *= qAngleChange;
}

glm::vec3 cGameObject::getEulerAngle(void)
{
	// In glm::gtx (a bunch of helpful things there)
	glm::vec3 EulerAngle = glm::eulerAngles(this->rotation);

	return EulerAngle;
}

void cGameObject::AddForce(glm::vec3 newtons)
{
	this->force += newtons;
}

void cGameObject::SetForce(glm::vec3 newtons)
{
	this->force = newtons;
}

glm::vec3 cGameObject::GetForce()
{
	return force;
}

glm::vec3 cGameObject::GetVelocity()
{
	return velocity;
}

void cGameObject::SetVelocity(glm::vec3 vel)
{
	velocity = vel;
}

glm::vec3 cGameObject::GetAcceleration()
{
	return acceleration;
}

void cGameObject::SetAcceleration(glm::vec3 accel)
{
	acceleration = accel;
}
