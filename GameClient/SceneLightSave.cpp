
#include "Scene.h"
#include "cLightManager.h"
#include <fstream>
#include "rapid_definitions.h"
#include <rapidxml/rapidxml_print.hpp>
#include <vector>
#include <iostream>
using namespace rapidxml;
using namespace std;
using namespace rapidxml;

xml_document<> doc;


void ApplyXYZW(xml_node<>* node, glm::vec4& vec)
{
	xml_attribute<>* attr = new xml_attribute<>();
	char* valueBuffer;

	attr->name("x");
	valueBuffer = doc.allocate_string(std::to_string(vec.x).c_str());
	attr->value(valueBuffer);
	node->append_attribute(attr);

	attr = new xml_attribute<>();
	attr->name("y");
	valueBuffer = doc.allocate_string(std::to_string(vec.y).c_str());
	attr->value(valueBuffer);
	node->append_attribute(attr);

	attr = new xml_attribute<>();
	attr->name("z");
	valueBuffer = doc.allocate_string(std::to_string(vec.z).c_str());
	attr->value(valueBuffer);
	node->append_attribute(attr);

	attr = new xml_attribute<>();
	attr->name("w");
	valueBuffer = doc.allocate_string(std::to_string(vec.w).c_str());
	attr->value(valueBuffer);
	node->append_attribute(attr);
}

bool Scene::SaveLights()
{
	doc.clear();

	xml_node<>* root_node;

	ifstream filestream("assets/scenes/" + this->SceneName);
	if (!filestream.is_open())
	{
		cout << "Scene could not find/open file: " << this->SceneName << endl;
	}

	vector<char> buffer((istreambuf_iterator<char>)(filestream), istreambuf_iterator<char>());
	buffer.push_back('\0');

	doc.parse<0>(&buffer[0]);

	root_node = doc.first_node("Scene");

	xml_node<>* light_node = root_node->first_node("Lights");
	if (light_node)
	{
		light_node->remove_all_nodes();


		for (unsigned int i = 0; i < this->pLightManager->Lights.size(); ++i)
		{
			sLight* light = pLightManager->Lights[i];
			xml_node<>* node = new xml_node<>(node_type::node_element);

			node->name("Light");

			xml_node<>* prop = new xml_node<>(node_type::node_element);
			prop->name("Position");
			ApplyXYZW(prop, light->position);
			node->append_node(prop);

			prop = new xml_node<>(node_type::node_element);
			prop->name("Diffuse");
			ApplyXYZW(prop, light->diffuse);
			node->append_node(prop);

			prop = new xml_node<>(node_type::node_element);
			prop->name("Specular");
			ApplyXYZW(prop, light->specular);
			node->append_node(prop);

			prop = new xml_node<>(node_type::node_element);
			prop->name("Atten");
			ApplyXYZW(prop, light->atten);
			node->append_node(prop);

			prop = new xml_node<>(node_type::node_element);
			prop->name("Direction");
			ApplyXYZW(prop, light->direction);
			node->append_node(prop);

			prop = new xml_node<>(node_type::node_element);
			prop->name("Param1");
			ApplyXYZW(prop, light->param1);
			node->append_node(prop);

			prop = new xml_node<>(node_type::node_element);
			prop->name("Param2");
			ApplyXYZW(prop, light->param2);
			node->append_node(prop);
			

			light_node->append_node(node);

		}


		ofstream output_stream("assets/scenes/" + this->SceneName);
		if (output_stream.is_open())
		{
			output_stream << "<?xml version=\"1.0\"?>\n";
			output_stream << doc;
			output_stream.close();
		}
	}

	filestream.close();

	return true;
}