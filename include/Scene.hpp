#ifndef __SCENE_HPP__
#define __SCENE_HPP__

#include "Node.hpp"
#include "Material.hpp"
#include "Texture.hpp"

#include <string>
#include <assimp/vector3.h>

struct BoundingBox
{
	aiVector3D min;
	aiVector3D max;
};

class Scene {
	private:
		void calculateBoundingBox(const aiScene *pScene, aiVector3D *pMin, aiVector3D *pMax, const aiNode *pNode);
		void loadNode(const aiScene* scene, aiNode* node, Node *parent, Node& newNode);
		Mesh* loadMesh(aiMesh* mesh);

		Node *m_Root;
		std::vector<float> m_Vertices;
		std::vector<float> m_Normals;
		std::vector<float> m_TexCoords;
		std::vector<unsigned int> m_Indices;

		std::vector<Material*> m_Materials;
		std::vector<Texture*> m_Textures;
		std::vector<Mesh*> m_Meshes;

		unsigned int	m_VertexArrayID,
						m_VertexBuffer,
						m_NormalBuffer,
						m_TexCoordBuffer;

		BoundingBox m_BoundingBox;

	public:
		Node *getRoot(void);
		bool load(const std::string& path, Node *pRoot);	// root = NULL

		BoundingBox getBoundingBox(void);

		Scene();
};

#endif