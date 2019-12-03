

#include "Scene.h"
#include "cGameObject.h"
#include <fstream>
#include "rapid_definitions.h"
#include <rapidxml/rapidxml_print.hpp>
#include <vector>
#include <iostream>
using namespace rapidxml;
using namespace std;
using namespace rapidxml;

xml_document<> layout_doc;

void _ApplyXYZW(xml_node<>* node, glm::vec4 vec)
{
	xml_attribute<>* attr = new xml_attribute<>();
	char* valueBuffer;

	attr->name("x");
	valueBuffer = layout_doc.allocate_string(std::to_string(vec.x).c_str());
	attr->value(valueBuffer);
	node->append_attribute(attr);

	attr = new xml_attribute<>();
	attr->name("y");
	valueBuffer = layout_doc.allocate_string(std::to_string(vec.y).c_str());
	attr->value(valueBuffer);
	node->append_attribute(attr);

	attr = new xml_attribute<>();
	attr->name("z");
	valueBuffer = layout_doc.allocate_string(std::to_string(vec.z).c_str());
	attr->value(valueBuffer);
	node->append_attribute(attr);

	attr = new xml_attribute<>();
	attr->name("w");
	valueBuffer = layout_doc.allocate_string(std::to_string(vec.w).c_str());
	attr->value(valueBuffer);
	node->append_attribute(attr);
}

void _ApplyXYZ(xml_node<>* node, glm::vec3 vec)
{
	xml_attribute<>* attr = new xml_attribute<>();
	char* valueBuffer;

	attr->name("x");
	valueBuffer = layout_doc.allocate_string(std::to_string(vec.x).c_str());
	attr->value(valueBuffer);
	node->append_attribute(attr);

	attr = new xml_attribute<>();
	attr->name("y");
	valueBuffer = layout_doc.allocate_string(std::to_string(vec.y).c_str());
	attr->value(valueBuffer);
	node->append_attribute(attr);

	attr = new xml_attribute<>();
	attr->name("z");
	valueBuffer = layout_doc.allocate_string(std::to_string(vec.z).c_str());
	attr->value(valueBuffer);
	node->append_attribute(attr);
}

bool Scene::SaveLayout()
{
	layout_doc.clear();

	xml_node<>* root_node;

	ifstream filestream("assets/scenes/" + this->SceneName);
	if (!filestream.is_open())
	{
		cout << "Scene could not find/open file: " << this->SceneName << endl;
	}

	vector<char> buffer((istreambuf_iterator<char>)(filestream), istreambuf_iterator<char>());
	buffer.push_back('\0');

	layout_doc.parse<0>(&buffer[0]);

	root_node = layout_doc.first_node("Scene");

	xml_node<>* layout_node = root_node->first_node("Layout");
	if (layout_node)
	{
		layout_node->remove_all_nodes();


		for (unsigned int i = 0; i < this->vecGameObjects.size(); ++i)
		{
			cGameObject* obj = vecGameObjects[i];
			xml_node<>* node = new xml_node<>(node_type::node_element);
			char* valueBuffer;

			node->name("GameObject");

			xml_attribute<>* attr = new xml_attribute<>();
			attr->name("mesh");
			valueBuffer = layout_doc.allocate_string(obj->meshName.c_str());
			attr->value(valueBuffer);
			node->append_attribute(attr);

			attr = new xml_attribute<>();
			attr->name("tag");
			valueBuffer = layout_doc.allocate_string(obj->tag.c_str());
			attr->value(valueBuffer);
			node->append_attribute(attr);

			attr = new xml_attribute<>();
			attr->name("shader");
			valueBuffer = layout_doc.allocate_string(obj->shaderName.c_str());
			attr->value(valueBuffer);
			node->append_attribute(attr);

			xml_node<>* prop = new xml_node<>(node_type::node_element);
			prop->name("Position");
			_ApplyXYZ(prop, obj->pos);
			node->append_node(prop);

			prop = new xml_node<>(node_type::node_element);
			prop->name("Rotation");
			glm::vec3 angle = obj->getEulerAngle();
			_ApplyXYZ(prop, angle);
			node->append_node(prop);

			prop = new xml_node<>(node_type::node_element);
			prop->name("Colour");
			_ApplyXYZW(prop, obj->colour);
			node->append_node(prop);

			prop = new xml_node<>(node_type::node_element);
			prop->name("SpecColour");
			_ApplyXYZ(prop, obj->specColour);
			node->append_node(prop);

			prop = new xml_node<>(node_type::node_element);
			prop->name("Velocity");
			_ApplyXYZ(prop, obj->GetVelocity());
			node->append_node(prop);

			prop = new xml_node<>(node_type::node_element);
			prop->name("Acceleration");
			_ApplyXYZ(prop, obj->GetAcceleration());
			node->append_node(prop);

			prop = new xml_node<>(node_type::node_element);
			prop->name("Scale");
			prop->value(layout_doc.allocate_string(std::to_string(obj->scale).c_str()));
			node->append_node(prop);

			prop = new xml_node<>(node_type::node_element);
			prop->name("SpecIntensity");
			prop->value(layout_doc.allocate_string(std::to_string(obj->specIntensity).c_str()));
			node->append_node(prop);

			prop = new xml_node<>(node_type::node_element);
			prop->name("IMass");
			prop->value(layout_doc.allocate_string(std::to_string(obj->inverseMass).c_str()));
			node->append_node(prop);

			prop = new xml_node<>(node_type::node_element);
			prop->name("Collider");
			switch (obj->Collider)
			{
			case MESH: prop->value("MESH"); break;
			case SPHERE: prop->value("SPHERE"); break;
			case CUBE: prop->value("CUBE"); break;
			case AABB: prop->value("AABB"); break;
			case POINT: prop->value("POINT"); break;
			default: prop->value("NONE"); break;
			}
			node->append_node(prop);

			for (int i = 0; i < 4; ++i)
			{
				prop = new xml_node<>(node_type::node_element);
				prop->name("Texture");
				attr = new xml_attribute<>();
				attr->name("ratio");
				attr->value(layout_doc.allocate_string(std::to_string(obj->textureRatio[i]).c_str()));
				prop->append_attribute(attr);
				prop->value(layout_doc.allocate_string(obj->texture[i].c_str()));
				node->append_node(prop);
			}

			for (int n = 0; n < obj->CollidePoints.size(); ++n)
			{
				prop = new xml_node<>(node_type::node_element);
				prop->name("CollidePoint");
				_ApplyXYZ(prop, obj->CollidePoints[n]);
				node->append_node(prop);
			}

			layout_node->append_node(node);

		}


		ofstream output_stream("assets/scenes/" + this->SceneName);
		if (output_stream.is_open())
		{
			output_stream << "<?xml version=\"1.0\"?>\n";
			output_stream << layout_doc;
			output_stream.close();
		}
	}

	filestream.close();

	return true;
}