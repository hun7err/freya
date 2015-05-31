#include "../include/Model.hpp"
#include <GL/glew.h>
#include <FreeImage.h>

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
	newMaterial->diffuseTexture = newMaterial->specularTexture = newMaterial->specularTexture = NULL;

	/*
	Type *a, b, c;	// no.
	HasTextures(), aiTexture -> embedded
	SOIL, DevIL -> too old
	ResIL -> problems and stuff
	glDrawElements instead of glDrawElementsBaseVertex
	meshes don't have transformation -> only nodes do
	exceptions, like std::out_of_range (why would you rely on them?)
	*/

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
	
	aiString path;
	unsigned int i;

	for (i = 0; i < material->GetTextureCount(aiTextureType_DIFFUSE); i++)
	{
		if(material->GetTexture(aiTextureType_DIFFUSE, i, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
		{
			TextureInfo *pTexture = loadTexture(path, aiTextureType_DIFFUSE);	// PE£NA œcie¿ka!

			if(pTexture)
			{
				m_Textures.push_back(pTexture);
				newMaterial->diffuseTexture = pTexture;
			}
		}
	}

	for (i = 0; i < material->GetTextureCount(aiTextureType_NORMALS); i++)
	{
		if(material->GetTexture(aiTextureType_NORMALS, i, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
		{
			TextureInfo *pTexture = loadTexture(path, aiTextureType_NORMALS);

			if(pTexture)
			{
				m_Textures.push_back(pTexture);
				newMaterial->normalTexture = pTexture;
			}
		}
	}

	for (i = 0; i < material->GetTextureCount(aiTextureType_SPECULAR); i++)
	{
		if(material->GetTexture(aiTextureType_SPECULAR, i, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
		{
			TextureInfo *pTexture = loadTexture(path, aiTextureType_SPECULAR);

			if(pTexture)
			{
				m_Textures.push_back(pTexture);
				newMaterial->diffuseTexture = pTexture;
			}
		}
	}

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

FIBITMAP* Model::loadImageToBitmap(const char *filename)
{
	FIBITMAP *bitmap = NULL;
	FREE_IMAGE_FORMAT format = FreeImage_GetFIFFromFilename(filename);

	bitmap = FreeImage_Load(format, filename, 0); // JPEG_DEFAULT?
	
	return bitmap;
}

TextureInfo* Model::loadTexture(aiString path, aiTextureType type)
{
	TextureInfo* newTexture = NULL;

	FIBITMAP *pBitmap = loadImageToBitmap(path.C_Str());
	if(pBitmap)
	{
		newTexture = new TextureInfo();

		switch(type)
		{
			case aiTextureType_DIFFUSE:
				newTexture->type = TEXTURE_DIFFUSE;
			break;
			case aiTextureType_NORMALS:
				newTexture->type = TEXTURE_NORMAL;
			break;
			case aiTextureType_SPECULAR:
				newTexture->type = TEXTURE_SPECULAR;
			break;
		}

		unsigned int width = FreeImage_GetWidth(pBitmap),
					height = FreeImage_GetHeight(pBitmap);

		glGenTextures(1, &newTexture->id);
		glBindTexture(GL_TEXTURE_2D, newTexture->id);
	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glGenerateMipmap(GL_TEXTURE_2D);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA8, GL_UNSIGNED_BYTE, FreeImage_ConvertTo8Bits(pBitmap));
		FreeImage_Unload(pBitmap);

		m_Textures.push_back(newTexture);
	}

	return newTexture;
}

Mesh* Model::loadMesh(aiMesh* mesh)
{
	static unsigned int i, lastIndexCount, vertexIndexOffset;
	static aiVector3D vertex;

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

void Model::loadNode(const aiScene* scene, aiNode* node, Node *parent, Node& newNode)
{
	//printf("loadNode()\n");

	memcpy(&newNode.transformation, &node->mTransformation, 16*sizeof(GLfloat));
	newNode.meshes.resize(node->mNumMeshes);
	unsigned int i;

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
	// tutaj skonczyles jak cos, nie ma za co ;-)
	unsigned int i = 0;
	
	if(scene->HasMeshes())
	{
		if(scene->HasMaterials())	// to bylo po "meshes loaded"
		{
			for(; i < scene->mNumMaterials; ++i)
			{
				printf("loading material... ");
				MaterialInfo* material = loadMaterial(scene->mMaterials[i]);
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

void Model::calculateBoundingBox(const aiScene *pScene, aiVector3D *pMin, aiVector3D *pMax, const aiNode *pNode)
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

BoundingBox Model::getBoundingBox(void)
{
	return m_BoundingBox;
}

void Model::drawNode(unsigned int programID, const glm::mat4& projection, const glm::mat4& view, glm::mat4& model, Node& pNode)
{
	unsigned int i = 0;
	//if(childrenCount == 0)
	//	model = pNode.transformation;	// identity
	//model = pNode.transformation;

	glm::mat4 VP = projection * view, MVP;

	for(; i < pNode.meshes.size(); i++)
	{
		glUseProgram(programID);

		MVP = VP * model;

		GLuint MatrixID = glGetUniformLocation(programID, "MVP");
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
		glVertexAttribPointer(	// vertices
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,     // x,y,z
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, m_NormalBuffer);
		glVertexAttribPointer(	// normals
			1,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,     // x,y,z
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, m_TexCoordBuffer);
		glVertexAttribPointer(	// texcoords
			2,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			2,     // u,v
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		if(pNode.meshes[i]->material->diffuseTexture)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, pNode.meshes[i]->material->diffuseTexture->id);
		}
		
		/*
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_Meshes[i]->material->normalTexture->id);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, m_Meshes[i]->material->specularTexture->id);
		*/

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pNode.meshes[i]->indexBuffer);
 
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
			pNode.meshes[i]->indexCount * 3,	// count
			GL_UNSIGNED_INT,
			(void*)( pNode.meshes[i]->indexOffset * 3 * sizeof(unsigned int) )	// index offset
		);
		
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
	}

	for(i = 0; i < pNode.nodes.size(); i++)
	{
		model = pNode.nodes[i].transformation * model;
		drawNode(programID, projection, view, model, pNode.nodes[i]);	// na pocz¹tku, transformation * model
	}
}

void Model::draw(unsigned int programID, const glm::mat4& projection, const glm::mat4& view)
{
	//glm::mat4 model(1.0f);
	
	glm::mat4 model = m_Root->transformation;
	drawNode(programID, projection, view, model, *m_Root);
}