#include "../include/Model.hpp"
#include <GL/glew.h>

MaterialInfo* Model::loadMaterial(aiMaterial* material)
{
	MaterialInfo* newMaterial = new MaterialInfo();

	int shadingModel;
	material->Get(AI_MATKEY_SHADING_MODEL, shadingModel);

	if(shadingModel != aiShadingMode_Phong && shadingModel != aiShadingMode_Gouraud)
	{
		newMaterial->isDefault = true;
		return newMaterial;
	}
	
	newMaterial->isDefault = true;

	aiColor3D	diffuse(0.0f,0.0f,0.0f),
				ambient(0.0f,0.0f,0.0f),
				specular(0.0f,0.0f,0.0f),
				emissive(0.0f,0.0f,0.0f);
	float shininess = 0.0f;

	material->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
	material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
	material->Get(AI_MATKEY_COLOR_SPECULAR, specular);
	material->Get(AI_MATKEY_COLOR_EMISSIVE, emissive);
	material->Get(AI_MATKEY_SHININESS, shininess);

	// to mo¿e siê posypaæ (wskaŸnik na r)
	newMaterial->ambient = glm::vec3(ambient.r, ambient.g, ambient.b);
	newMaterial->diffuse = glm::vec3(diffuse.r, diffuse.g, diffuse.b);
	newMaterial->specular = glm::vec3(specular.r, specular.g, specular.b);
	newMaterial->emissive = glm::vec3(emissive.r, emissive.g, emissive.b);
	newMaterial->shininess = shininess;

	newMaterial->ambient *= 2;
	if(newMaterial->shininess == 0.0f)
		newMaterial->shininess = 30.0f;

	return newMaterial;
}

Mesh* Model::loadMesh(aiMesh* mesh)
{
	static unsigned int i, lastIndexCount, vertexIndexOffset;
	static aiVector3D vertex;

	Mesh* newMesh = new Mesh();
	newMesh->indexOffset = m_Indices.size();
	lastIndexCount = m_Indices.size();
	vertexIndexOffset = m_Vertices.size()/3;

	if(mesh->mNumVertices > 0)
	{
		for(i = 0; i < mesh->mNumVertices; ++i)
		{
			vertex = mesh->mVertices[i];

			m_Vertices.push_back(vertex.x);
			m_Vertices.push_back(vertex.y);
			m_Vertices.push_back(vertex.z);
		}
	}

	if(mesh->HasNormals())
	{
		for(i = 0; i < mesh->mNumVertices; ++i)
		{
			vertex = mesh->mNormals[i];

			m_Normals.push_back(vertex.x);
			m_Normals.push_back(vertex.y);
			m_Normals.push_back(vertex.z);
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

void Model::loadNode(const aiScene* scene, aiNode* node, Node *parent, Node& newNode)
{
	printf("loadNode()\n");

	memcpy(&newNode.transformation, &node->mTransformation, 16*sizeof(GLfloat));
	newNode.meshes.resize(node->mNumMeshes);
	static unsigned int i;

	for(i = 0; i < node->mNumMeshes; ++i)
	{
		Mesh* mesh = m_Meshes[node->mMeshes[i]];
		newNode.meshes[i] = mesh;
	}

	for(i = 0; i < node->mNumChildren; ++i)
	{
		newNode.nodes.push_back(Node());
		loadNode(scene, node->mChildren[i], parent, newNode.nodes[i]);
	}
}

bool Model::load(const std::string& fileName)
{
	printf("importing scene... ");
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(fileName,
		aiProcess_GenSmoothNormals |
		aiProcess_CalcTangentSpace |
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_SortByPType
	);
	printf("done\n");

	if(!scene)
	{
		fprintf(stderr, "Error loading file '%s': %s\n", fileName.c_str(), importer.GetErrorString());
		return nullptr;
	}

	static unsigned int i;

	if(scene->HasMaterials())
	{
		for(i = 0; i < scene->mNumMaterials; ++i)
		{
			printf("loading material... ");
			MaterialInfo* material = loadMaterial(scene->mMaterials[i]);
			m_Materials.push_back(material);
			printf("done\n");
		}
		printf("materials loaded\n");
	}

	if(scene->HasMeshes())
	{
		for(i = 0; i < scene->mNumMeshes; ++i)
		{
			printf("loading mesh... ");
			m_Meshes.push_back(loadMesh(scene->mMeshes[i]));
			printf("done\n");
		}
		printf("meshes loaded\n");
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

	return true;
}

void Model::draw(void)
{
	static unsigned int i;

	for(i = 0; i < m_Meshes.size(); i++)
	{
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
		
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,     // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, m_NormalBuffer);
		
		glVertexAttribPointer(
			1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,     // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Meshes[i]->indexBuffer);
 
		/*
		glDrawElementsBaseVertex(
			GL_TRIANGLES,
			m_Meshes[i]->indexCount,
			GL_UNSIGNED_INT,
			0,
			m_Meshes[i]->indexOffset
		);
		*/
		
		glDrawElements(
			GL_TRIANGLES,
			m_Meshes[i]->indexCount * 3,
			GL_UNSIGNED_INT,
			(void*)( m_Meshes[i]->indexOffset * 3 * sizeof(unsigned int) )
		);
		

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		// (m_Meshes[i]->indexOffset * 3 * sizeof(unsigned int))

		//glDrawArrays(GL_TRIANGLES, 0, m_Vertices.size());
	}
}