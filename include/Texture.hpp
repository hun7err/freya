#ifndef __TEXTURE_HPP__
#define __TEXTURE_HPP__

#include <string>
#include <assimp/scene.h>

#define TEXTURE_TYPE_DIFFUSE	1
#define TEXTURE_TYPE_SPECULAR	2
#define TEXTURE_TYPE_NORMAL		3
#define TEXTURE_TYPE_UNDEFINED	255

class Texture {
	public:
		unsigned int id;
		unsigned char type;

		bool load(const std::string& path, aiTextureType texType);

		Texture(char type);
};

#endif // __TEXTURE_HPP__