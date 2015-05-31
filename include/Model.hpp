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
#include <FreeImage.h>

#define TEXTURE_DIFFUSE		1
#define TEXTURE_SPECULAR	2
#define TEXTURE_NORMAL		3

struct TextureInfo
{
	unsigned int id;
	unsigned char type;
};

struct MaterialInfo
{
	bool isDefault;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
	glm::vec3 emissive;
    float shininess;

	TextureInfo *diffuseTexture,
				 *specularTexture,
				 *normalTexture;
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
		TextureInfo* loadTexture(aiString path, aiTextureType type);
		FIBITMAP *loadImageToBitmap(const char *filename);
		void calculateBoundingBox(const aiScene *pScene, aiVector3D *pMin, aiVector3D *pMax, const aiNode *pNode);

		Mesh* loadMesh(aiMesh* mesh);
		void loadNode(const aiScene* scene, aiNode* node, Node *parent, Node& newNode);

		void drawNode(unsigned int programID, const glm::mat4& projection, const glm::mat4& view, glm::mat4& model, Node& pNode);

		std::vector<float> m_Vertices;
		std::vector<float> m_Normals;
		std::vector<float> m_TexCoords;
		std::vector<unsigned int> m_Indices;

		std::vector<MaterialInfo*> m_Materials;
		std::vector<TextureInfo*> m_Textures;
		std::vector<Mesh*> m_Meshes;
		Node* m_Root;
		unsigned int	m_VertexArrayID,
						m_VertexBuffer,
						m_NormalBuffer,
						m_TexCoordBuffer;

		BoundingBox m_BoundingBox;
	public:
		bool load(const std::string& fileName);
		BoundingBox getBoundingBox(void);
		void draw(unsigned int programID, const glm::mat4& projection, const glm::mat4& view);
};

#endif // __MODEL_HPP__