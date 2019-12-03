#ifndef _cGameObject_HG_
#define _cGameObject_HG_

#include <glm/glm.hpp>
#include <glm/vec3.hpp>
#include <glm/gtx/quaternion.hpp>
#include <string>
#include <vector>

class Scene;

enum ColliderType
{
	SPHERE,
	CUBE,
	MESH,
	POINT,
	AABB,
	NONE
};

class cGameObject
{
private:
	glm::quat rotation;		// Orientation as a quaternion
	glm::vec3 velocity;
	glm::vec3 acceleration;
	glm::vec3 force;

public:

	cGameObject();
	
	virtual ~cGameObject();

	std::string meshName = "";
	std::string tag = "";

	std::string shaderName = "basic";

	glm::vec3  pos = glm::vec3(0.f, 0.f, 0.f);
	glm::vec3  previousPos = glm::vec3(0.f, 0.f, 0.f);
	glm::vec4  colour = glm::vec4(1.f, 1.f, 1.f, 1.f);
	float scale = 1.f;

	glm::vec3 specColour = glm::vec3(.5f, .5f, .5f);
	float specIntensity = 1.f;
	bool uniformColour = false;
	bool isWireframe = false;

	float gravityScale = 0.f;
	float inverseMass = 1.f;

	ColliderType Collider = ColliderType::NONE;
	std::vector<glm::vec3> CollidePoints;
	bool isCollided = false;

	std::string texture[4];
	float textureRatio[4];

	std::vector<cGameObject*> children;

	virtual void UpdateLogic(Scene* scene) {}

	glm::mat4 ModelMatrix();
	glm::mat4 TranslationMatrix();
	glm::mat4 RotationMatrix();
	glm::mat4 ScaleMatrix();

	glm::quat getQOrientation(void);

	// Overwrite the orientation
	void setOrientation(glm::vec3 EulerAngleDegreesXYZ);
	void setOrientation(glm::quat qAngle);
	// Updates the existing angle
	void updateOrientation(glm::vec3 EulerAngleDegreesXYZ);
	void updateOrientation(glm::quat qAngle);
	glm::vec3 getEulerAngle(void);

	void AddForce(glm::vec3 newtons);
	void SetForce(glm::vec3 newtons);
	glm::vec3 GetForce();

	glm::vec3 GetVelocity();
	void SetVelocity(glm::vec3 vel);
	
	glm::vec3 GetAcceleration();
	void SetAcceleration(glm::vec3 accel);

};

#endif


