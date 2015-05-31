#ifndef __NODE_HPP__
#define __NODE_HPP__

#include <glm/glm.hpp>
#include "Mesh.hpp"

#include <vector>
#include <string>

class Node {
	friend class Scene;

	private:
		std::vector<Mesh*> m_Meshes;
		std::vector<Node> m_Nodes;

	public:
		bool load(const aiScene* scene, aiNode* node, Node *parent, Node& newNode);
		glm::mat4 transformation;
};

#endif // __NODE_HPP__