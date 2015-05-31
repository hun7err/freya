#include "../include/AssetManager.hpp"

/*
	do devloga:
	- opisac dlaczego AssetManager
	- opisac dlaczego nie Asset jako nadklasa (performance)
	- ciekawie pisac, do cholery! i nie spieszyc sie, piszac na kolanie
	- opisac strukture projektu
	- dlaczego premature optimization jest zle -> statyczne zmienne itp.
	- jak beda jakies problemy to je opisac
	- jak cos powstanie to screen czy cos, nawet jak blednie dziala
	- audiolog?
	- metody statyczne a performance?
	- odwolania do funkcji/metod a performance?
*/

bool AssetManager::load(const std::string& path, unsigned char assetType) {
	switch(assetType) {
		case ASSET_TYPE_MESH:
			return loadMesh(path);
		case ASSET_TYPE_MATERIAL:
			return loadMaterial(path);
		case ASSET_TYPE_TEXTURE:
			return loadTexture(path);
		default:
			return false;
	}
}

bool loadMesh(const std::string& path) {
	Mesh *pMesh = new Mesh();
	bool returnValue = pMesh->load(path);

	if(!returnValue) {
		delete pMesh;
		return false;
	} else {
		return returnValue;
	}
}

bool loadMaterial(const std::string& path) {
	Material *pMaterial = new Material();
	bool returnValue = pMaterial->load(path);

	if(!returnValue) {
		delete pMaterial;
		return false;
	} else {
		return returnValue;
	}
}

bool loadMaterial(aiMaterial *pAssimpMaterial) {
	Material *pMaterial = new Material();
	bool returnValue = pMaterial->load(pAssimpMaterial);

	if(!returnValue) {
		delete pMaterial;
		return false;
	} else {
		return returnValue;
	}
}

bool loadTexture(const std::string& path) {
}

void AssetManager::add(Material *pMaterial) {
	m_Materials.push_back(pMaterial);
}

void AssetManager::add(Texture *pTexture) {
	m_Textures.push_back(pTexture);
}

void AssetManager::add(Mesh *pMesh) {
	m_Meshes.push_back(pMesh);
}