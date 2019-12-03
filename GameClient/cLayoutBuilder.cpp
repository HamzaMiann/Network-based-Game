
#include "cLayoutBuilder.h"
#include "cGameObject.h"
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
using namespace rapidxml;

void setXYZ(glm::vec3& vec3, xml_node<>* node)
{
	vec3.x = strtof(node->first_attribute("x")->value(), 0);
	vec3.y = strtof(node->first_attribute("y")->value(), 0);
	vec3.z = strtof(node->first_attribute("z")->value(), 0);
}

void setXYZW(glm::vec4& vec4, xml_node<>* node)
{
	vec4.x = strtof(node->first_attribute("x")->value(), 0);
	vec4.y = strtof(node->first_attribute("y")->value(), 0);
	vec4.z = strtof(node->first_attribute("z")->value(), 0);
	vec4.w = strtof(node->first_attribute("w")->value(), 0);
}

void cLayoutBuilder::Build(Scene& scene, rapidxml::xml_node<>* node)
{
	printf("Building Game Objects...\n");

	for (xml_node<>* object_node = node->first_node(); object_node; object_node = object_node->next_sibling())
	{
		cGameObject* ptr = nullptr;
		std::string objectName = object_node->name();
		xml_attribute<>* meshAttr = object_node->first_attribute("mesh");
		xml_attribute<>* tagAttr = object_node->first_attribute("tag");
		std::string shader = object_node->first_attribute("shader")->value();
		if (objectName == "GameObject")
		{
			ptr = ptr = new cGameObject();
			if (meshAttr) ptr->meshName = meshAttr->value();
			if (tagAttr) ptr->tag = tagAttr->value();
			ptr->shaderName = shader;
			int texture_id = 0;
			for (xml_node<>* property_node = object_node->first_node();
				 property_node;
				 property_node = property_node->next_sibling())
			{
				std::string propName = property_node->name();
				if (propName == "Position")
				{
					setXYZ(ptr->pos, property_node);
				}
				else if (propName == "Rotation")
				{
					glm::vec3 orientation(0.f);
					setXYZ(orientation, property_node);
					ptr->setOrientation(orientation);
				}
				else if (propName == "Colour")
				{
					setXYZW(ptr->colour, property_node);
				}
				else if (propName == "Velocity")
				{
					glm::vec3 vel = ptr->GetVelocity();
					setXYZ(vel, property_node);
					ptr->SetVelocity(vel);
				}
				else if (propName == "Acceleration")
				{
					glm::vec3 accel = ptr->GetAcceleration();
					setXYZ(accel, property_node);
					ptr->SetAcceleration(accel);
				}
				else if (propName == "Scale")
				{
					ptr->scale = strtof(property_node->value(),0);
				}
				else if (propName == "IMass")
				{
					ptr->inverseMass = strtof(property_node->value(), 0);
				}
				else if (propName == "SpecIntensity")
				{
					ptr->specIntensity = strtof(property_node->value(), 0);
				}
				else if (propName == "SpecColour")
				{
					setXYZ(ptr->specColour, property_node);
				}
				else if (propName == "CollidePoint")
				{
					glm::vec3 point(0.f);
					setXYZ(point, property_node);
					ptr->CollidePoints.push_back(point);
				}
				else if (propName == "Collider")
				{
					std::string collider = property_node->value();
					if (collider == "SPHERE")
					{
						ptr->Collider = SPHERE;
					}
					else if (collider == "MESH")
					{
						ptr->Collider = MESH;
					}
					else if (collider == "AABB")
					{
						ptr->Collider = AABB;
					}
					else if (collider == "POINT")
					{
						ptr->Collider = POINT;
					}
					else
					{
						ptr->Collider = NONE;
					}
				}
				else if (propName == "Texture")
				{
					ptr->textureRatio[texture_id] = strtof(property_node->first_attribute("ratio")->value(), 0);
					ptr->texture[texture_id++] = property_node->value();
				}
			}
		}

		if (ptr)
			scene.vecGameObjects.push_back(ptr);
	}
}
