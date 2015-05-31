#ifndef __MATERIAL_HPP__
#define __MATERIAL_HPP__

#include "Texture.hpp"

#include <glm/glm.hpp>

struct Material {
	bool isDefault;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
	glm::vec3 emissive;
    float shininess;

	Texture *diffuseTexture,
			*specularTexture,
			*normalTexture;

	bool load(aiMaterial* pMaterial);
	bool load(const std::string& path);	// do wlasnych materialow
};

#endif // __MATERIAL_HPP__