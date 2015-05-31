#include "../include/Scene.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <GL/glew.h>

Node* Scene::getRoot(void) {
	return m_Root;
}

bool Scene::load(const std::string& path, Node* pRoot = NULL) {
	if(!pRoot)
		pRoot = m_Root;

	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path,
		aiProcess_GenSmoothNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType
	);

	unsigned int i = 0;
	
	if(scene->HasMeshes())
	{
		if(scene->HasMaterials())	// to bylo po "meshes loaded"
		{
			for(; i < scene->mNumMaterials; ++i)
			{
				printf("loading material... ");
				Material* material = loadMaterial(scene->mMaterials[i]);
				m_Materials.push_back(material);
				printf("done\n");
			}
			printf("materials loaded\n");
		}

		for(i = 0; i < scene->mNumMeshes; ++i)
		{
			printf("loading mesh... ");
			m_Meshes.push_back(loadMesh(scene->mMeshes[i]));
			printf("done\n");
		}
		
		printf("%d meshes loaded from scene\n", scene->mNumMeshes);
	}
	else
	{
		fprintf(stderr, "No meshes found in '%s'\n", fileName.c_str());
		return false;
	}

	if(scene->mRootNode)
	{
		m_Root = new Node();
		this->loadNode(scene, scene->mRootNode, 0, *m_Root);

		calculateBoundingBox(scene, &m_BoundingBox.min, &m_BoundingBox.max, scene->mRootNode);
		printf("min: [%f, %f, %f]\n", m_BoundingBox.min.x, m_BoundingBox.min.y, m_BoundingBox.min.z);
		printf("max: [%f, %f, %f]\n", m_BoundingBox.max.x, m_BoundingBox.max.y, m_BoundingBox.max.z);
	}
	else
	{
		return false;
	}

	glGenVertexArrays(1, &m_VertexArrayID);
	glBindVertexArray(m_VertexArrayID);

	glGenBuffers(1, &m_VertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(float), &m_Vertices[0], GL_STATIC_DRAW);
	
	glGenBuffers(1, &m_NormalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_NormalBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_Normals.size() * sizeof(float), &m_Normals[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_TexCoordBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_TexCoordBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_TexCoords.size() * sizeof(float), &m_TexCoords[0], GL_STATIC_DRAW);

	return true;
}

void Scene::loadNode(const aiScene* scene, aiNode* node, Node *parent, Node& newNode) {
	memcpy(&newNode.transformation, &node->mTransformation, 16*sizeof(GLfloat));
	newNode.m_Meshes.resize(node->mNumMeshes);
	unsigned int i;

	for(i = 0; i < node->mNumMeshes; ++i) {
		Mesh* mesh = m_Meshes[node->mMeshes[i]];
		newNode.m_Meshes[i] = mesh;
	}

	for(i = 0; i < node->mNumChildren; ++i) {
		newNode.m_Nodes.push_back(Node());
		loadNode(scene, node->mChildren[i], parent, newNode.nodes[i]);
	}
}

Mesh* Scene::loadMesh(aiMesh* mesh) {
	unsigned int i, lastIndexCount, vertexIndexOffset;
	aiVector3D vertex;

	Mesh* newMesh = new Mesh();
	newMesh->indexOffset = m_Indices.size();
	lastIndexCount = m_Indices.size();
	vertexIndexOffset = m_Vertices.size()/3;

	bool loadNormals = mesh->HasNormals(),
		loadUVs = mesh->HasTextureCoords(0);

	if(mesh->mNumVertices > 0)
	{
		for(i = 0; i < mesh->mNumVertices; ++i)
		{
			vertex = mesh->mVertices[i];

			m_Vertices.push_back(vertex.x);
			m_Vertices.push_back(vertex.y);
			m_Vertices.push_back(vertex.z);

			if(loadNormals)
			{
				vertex = mesh->mNormals[i];

				m_Normals.push_back(vertex.x);
				m_Normals.push_back(vertex.y);
				m_Normals.push_back(vertex.z);
			}

			if(loadUVs)
			{
				vertex = mesh->mTextureCoords[0][i];

				m_TexCoords.push_back(vertex.x);
				m_TexCoords.push_back(vertex.y);
			}
		}
	}

	for(i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace* face = &mesh->mFaces[i];
		
		if(face->mNumIndices != 3)
			continue;

		m_Indices.push_back(face->mIndices[0]+vertexIndexOffset);
		m_Indices.push_back(face->mIndices[1]+vertexIndexOffset);
		m_Indices.push_back(face->mIndices[2]+vertexIndexOffset);
	}

	newMesh->indexCount = m_Indices.size() - lastIndexCount;
	newMesh->material = m_Materials.at(mesh->mMaterialIndex);

	glGenBuffers(1, &newMesh->indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, newMesh->indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, newMesh->indexCount * sizeof(unsigned int), &m_Indices[newMesh->indexOffset], GL_STATIC_DRAW);

	return newMesh;
}

void Scene::calculateBoundingBox(const aiScene *pScene, aiVector3D *pMin, aiVector3D *pMax, const aiNode *pNode)
{
	unsigned int n = 0, t;
	static aiVector3D tmp(0);

	for (; n < pNode->mNumMeshes; ++n)
	{
		const aiMesh* mesh = pScene->mMeshes[pNode->mMeshes[n]];
		for (t = 0; t < mesh->mNumVertices; ++t)
		{
			tmp = mesh->mVertices[t];

			pMin->x = min(pMin->x,tmp.x);
			pMin->y = min(pMin->y,tmp.y);
			pMin->z = min(pMin->z,tmp.z);

			pMax->x = max(pMax->x,tmp.x);
			pMax->y = max(pMax->y,tmp.y);
			pMax->z = max(pMax->z,tmp.z);
		}
	}

	for(n = 0; n < pNode->mNumChildren; ++n)
	{
		calculateBoundingBox(pScene, pMin, pMax, pNode->mChildren[n]);
	}
}