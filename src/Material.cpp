#include "../include/Material.hpp"

bool Material::load(aiMaterial* pMaterial) {
	int shadingModel;
	pMaterial->Get(AI_MATKEY_SHADING_MODEL, shadingModel);
	
	this->isDefault = true;

	if(shadingModel != aiShadingMode_Phong && shadingModel != aiShadingMode_Gouraud)
	{
		return this;
	}
	
	this->diffuseTexture = this->specularTexture = this->specularTexture = NULL;

	aiColor3D	diffuse(0.0f,0.0f,0.0f),
				ambient(0.0f,0.0f,0.0f),
				specular(0.0f,0.0f,0.0f),
				emissive(0.0f,0.0f,0.0f);
	float shininess = 0.0f;

	pMaterial->Get(AI_MATKEY_COLOR_AMBIENT, ambient);
	pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse);
	pMaterial->Get(AI_MATKEY_COLOR_SPECULAR, specular);
	pMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, emissive);
	pMaterial->Get(AI_MATKEY_SHININESS, shininess);
	
	aiString path;
	unsigned int i;

	for (i = 0; i < pMaterial->GetTextureCount(aiTextureType_DIFFUSE); i++)
	{
		if(pMaterial->GetTexture(aiTextureType_DIFFUSE, i, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
		{
			TextureInfo *pTexture = loadTexture(path, aiTextureType_DIFFUSE);	// PE£NA œcie¿ka!

			if(pTexture)
			{
				m_Textures.push_back(pTexture);
				this->diffuseTexture = pTexture;
			}
		}
	}

	for (i = 0; i < pMaterial->GetTextureCount(aiTextureType_NORMALS); i++)
	{
		if(pMaterial->GetTexture(aiTextureType_NORMALS, i, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
		{
			TextureInfo *pTexture = loadTexture(path, aiTextureType_NORMALS);

			if(pTexture)
			{
				m_Textures.push_back(pTexture);
				this->normalTexture = pTexture;
			}
		}
	}

	for (i = 0; i < pMaterial->GetTextureCount(aiTextureType_SPECULAR); i++)
	{
		if(pMaterial->GetTexture(aiTextureType_SPECULAR, i, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
		{
			Texture *pTexture = new Texture(TEXTURE_TYPE_SPECULAR);
			pTexture->load(path, aiTextureType_SPECULAR);

			if(pTexture)
			{
				m_Textures.push_back(pTexture);
				this->diffuseTexture = pTexture;
			}
		}
	}

	this->ambient = glm::vec3(ambient.r, ambient.g, ambient.b);
	this->diffuse = glm::vec3(diffuse.r, diffuse.g, diffuse.b);
	this->specular = glm::vec3(specular.r, specular.g, specular.b);
	this->emissive = glm::vec3(emissive.r, emissive.g, emissive.b);
	this->shininess = shininess;

	this->ambient *= 2;
	if(this->shininess == 0.0f)
		this->shininess = 30.0f;

	return this;

}