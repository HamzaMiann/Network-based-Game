#pragma once

#include <glm/vec3.hpp>
#include <vector>
#include "cAABB.h"
#include "cMesh.h"


class octree
{
public:

	struct octree_node
	{
		cAABB* AABB;
		octree_node* nodes[8];
		bool has_nodes = true;
		bool has_triangles = false;

		~octree_node();

		std::vector<const sMeshTriangle*> triangles;

	}* main_node = 0;

	octree() {}
	~octree() {}

	void generate_tree(glm::vec3 min, float length);
	void attach_triangles(std::vector<sMeshTriangle>* triangles);

	bool attach_triangles_by_node(std::vector<sMeshTriangle> const& triangles, octree::octree_node* node);

	octree_node* find_node(glm::vec3 const& point);

private:
	octree_node* _generate(int depth, glm::vec3 min, float length);
	bool _attach(std::vector<sMeshTriangle> const& triangles, octree::octree_node* node);
	octree_node* _find(glm::vec3 const& point, octree_node* node);
};


