#ifndef __ASSET_MANAGER_HPP__
#define __ASSET_MANAGER_HPP__

#include "Material.hpp"
#include "Texture.hpp"
#include "Mesh.hpp"

#include <vector>

#define ASSET_TYPE_MESH		0
#define ASSET_TYPE_MATERIAL	1
#define ASSET_TYPE_TEXTURE	2

class AssetManager {
	friend class Material;

	private:
		std::vector<Mesh*> m_Meshes;
		std::vector<Material*> m_Materials;
		std::vector<Texture*> m_Textures;

		bool loadMesh(const std::string& path);
		bool loadMaterial(const std::string& material);
		bool loadTexture(const std::string& path);

	public:
		bool load(const std::string& path, unsigned char assetType);
		bool load(aiMaterial *pAssimpMaterial);

		void add(Material *pMaterial);
		void add(Texture *pTexture);
		void add(Mesh *pMesh);
};

#endif