

#include "octree.h"
#include <thread>
#define DEPTH 5

void _octree_attach_triangles(std::vector<sMeshTriangle>* triangles, octree* tree, unsigned int from, unsigned int to)
{
	for (from; from < to; ++from)
	{
		tree->attach_triangles_by_node(*triangles, tree->main_node->nodes[from]);
	}
	printf("Finished :D\n");
}

octree::octree_node* octree::_generate(int depth, glm::vec3 min, float length)
{
	if (depth > DEPTH)
		return nullptr;

	octree::octree_node* node = new octree::octree_node;

	node->AABB = new cAABB;
	node->AABB->min = min;
	node->AABB->length = length;

	if (depth == DEPTH) node->has_nodes = false;

	float half = length / 2.f;

	node->nodes[0] = _generate(depth + 1,
							   min,
							   length / 2.f);

	node->nodes[1] = _generate(depth + 1,
							   min + glm::vec3(half, 0.f, 0.f),
							   length / 2.f);

	node->nodes[2] = _generate(depth + 1,
							   min + glm::vec3(0.f, half, 0.f),
							   length / 2.f);

	node->nodes[3] = _generate(depth + 1,
							   min + glm::vec3(half, half, 0.f),
							   length / 2.f);

	node->nodes[4] = _generate(depth + 1,
							   min + glm::vec3(0.f, 0.f, half),
							   length / 2.f);

	node->nodes[5] = _generate(depth + 1,
							   min + glm::vec3(0.f, half, half),
							   length / 2.f);

	node->nodes[6] = _generate(depth + 1,
							   min + glm::vec3(half, 0.f, half),
							   length / 2.f);

	node->nodes[7] = _generate(depth + 1,
							   min + glm::vec3(half, half, half),
							   length / 2.f);

	return node;
}

void octree::generate_tree(glm::vec3 min, float length)
{
	printf("Generating octree...\n");
	main_node = _generate(1, min, length);
}

void octree::attach_triangles(std::vector<sMeshTriangle>* triangles)
{
	//this->_attach(triangles, main_node);
	std::thread thread1(_octree_attach_triangles, triangles, this, 0, 3);
	std::thread thread2(_octree_attach_triangles, triangles, this, 3, 6);
	std::thread thread3(_octree_attach_triangles, triangles, this, 6, 8);

	printf("joining thread1...\n");
	if (!thread1.joinable()) exit(1);
	thread1.join();
	printf("joining thread2...\n");
	if (!thread2.joinable()) exit(1);
	thread2.join();
	printf("joining thread3...\n");
	if (!thread3.joinable()) exit(1);
	thread3.join();
	printf("Done!...\n");

	main_node->has_nodes = true;
	main_node->has_triangles = true;

	/*int i = 0;
	for (; i < 8; ++i)
	{
		threads[i] = new std::thread(_octree_attach_triangles, triangles, this, main_node->nodes[i]);
	}
	i = 0;
	for (; i < 8; ++i)
	{
		if (threads[i])
			threads[i]->join();
		main_node->has_triangles |= main_node->nodes[i]->has_triangles;
		main_node->has_nodes |= main_node->nodes[i]->has_triangles;
		delete threads[i];
	}*/

}

bool octree::attach_triangles_by_node(std::vector<sMeshTriangle> const& triangles, octree::octree_node* node)
{
	return this->_attach(triangles, node);
}

bool octree::_attach(std::vector<sMeshTriangle> const& triangles, octree::octree_node* node)
{
	if (node && node->has_nodes)
	{
		bool hasTriangles = false;
		for (int i = 0; i < 8; ++i)
		{
			hasTriangles |= _attach(triangles, node->nodes[i]);
		}
		node->has_triangles = hasTriangles;
		if (!node->has_triangles)
		{
			for (int n = 0; n < 8; ++n)
			{
				delete node->nodes[n];
				node->nodes[n] = NULL;
			}
			node->has_nodes = false;
		}
		return hasTriangles;
	}
	else if (node)
	{
		//printf("%d node\n", i++);
		bool hasTriangles = false;
		for (int i = 0; i < triangles.size(); ++i)
		{
			if (node->AABB->contains(triangles[i].first))
			{
				hasTriangles = true;
				node->triangles.push_back(&triangles[i]);
			}
			else if (node->AABB->contains(triangles[i].second))
			{
				hasTriangles = true;
				node->triangles.push_back(&triangles[i]);
			}
			else if (node->AABB->contains(triangles[i].third))
			{
				hasTriangles = true;
				node->triangles.push_back(&triangles[i]);
			}
			else if (node->AABB->contains(triangles[i].m1))
			{
				hasTriangles = true;
				node->triangles.push_back(&triangles[i]);
			}
			else if (node->AABB->contains(triangles[i].m2))
			{
				hasTriangles = true;
				node->triangles.push_back(&triangles[i]);
			}
			else if (node->AABB->contains(triangles[i].m3))
			{
				hasTriangles = true;
				node->triangles.push_back(&triangles[i]);
			}
			else if (node->AABB->contains(triangles[i].m))
			{
				hasTriangles = true;
				node->triangles.push_back(&triangles[i]);
			}
		}
		node->has_triangles = hasTriangles;
		return hasTriangles || node->triangles.size() > 0;
	}
	return false;
}

octree::octree_node::~octree_node()
{
	for (int i = 0; i < 8; ++i)
	{
		if (this->nodes[i])
		{
			delete this->nodes[i];
			this->nodes[i] = nullptr;
			this->has_nodes = false;
		}
	}
}


octree::octree_node* octree::find_node(glm::vec3 const& point)
{
	return _find(point, main_node);
}

octree::octree_node* octree::_find(glm::vec3 const& point, octree_node* node)
{
	if (!node->has_nodes) return node;
	if (node->AABB->contains(point))
	{
		for (int i = 0; i < 8; ++i)
		{
			if (node->nodes[i]->AABB->contains(point))
			{
				return _find(point, node->nodes[i]);
			}
		}
		return node;
	}
	return nullptr;
}