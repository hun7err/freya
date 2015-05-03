#ifndef __MODEL_HPP__
#define __MODEL_HPP__

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing fla

using namespace glm;

#include <string>
#include <vector>

struct MaterialInfo
{
	bool isDefault;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
	glm::vec3 emissive;
    float shininess;
};

struct Mesh
{
    unsigned int indexCount;
    unsigned int indexOffset;
    MaterialInfo* material;
	unsigned int indexBuffer;
};

struct Node
{
	glm::mat4 transformation;
    std::vector<Mesh*> meshes;
    std::vector<Node> nodes;
};

class Model
{
	private:
		MaterialInfo* loadMaterial(aiMaterial* material);
		Mesh* loadMesh(aiMesh* mesh);
		void loadNode(const aiScene* scene, aiNode* node, Node *parent, Node& newNode);

		std::vector<float> m_Vertices;
		std::vector<float> m_Normals;
		std::vector<unsigned int> m_Indices;

		std::vector<MaterialInfo*> m_Materials;
		std::vector<Mesh*> m_Meshes;
		Node* m_Root;
		unsigned int	m_VertexArrayID,
						m_VertexBuffer,
						m_NormalBuffer;

	public:
		bool load(const std::string& fileName);
		void draw(void);
};

#endif // __MODEL_HPP__