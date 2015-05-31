#include "../include/Texture.hpp"
#include <FreeImage.h>
#include <GL/glew.h>

Texture::Texture(char type = 255) {
	this->type = type;
}

bool Texture::load(const std::string& path, aiTextureType texType) {
	FIBITMAP *pBitmap = NULL;
	const char* pPath = path.c_str();
	FREE_IMAGE_FORMAT format = FreeImage_GetFIFFromFilename(pPath);

	pBitmap = FreeImage_Load(format, pPath, JPEG_DEFAULT); // JPEG_DEFAULT?
	
	if(pBitmap) {
		switch(type) {
			case aiTextureType_DIFFUSE:
				this->type = TEXTURE_TYPE_DIFFUSE;
			break;
			case aiTextureType_NORMALS:
				this->type = TEXTURE_TYPE_NORMAL;
			break;
			case aiTextureType_SPECULAR:
				this->type = TEXTURE_TYPE_SPECULAR;
			break;
			default:
				this->type = TEXTURE_TYPE_UNDEFINED;
		}

		unsigned int width = FreeImage_GetWidth(pBitmap),
					height = FreeImage_GetHeight(pBitmap);

		glGenTextures(1, &this->id);
		glBindTexture(GL_TEXTURE_2D, this->id);
	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glGenerateMipmap(GL_TEXTURE_2D);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA8, GL_UNSIGNED_BYTE, FreeImage_ConvertTo8Bits(pBitmap));
		FreeImage_Unload(pBitmap);
	} else return false;

	return true;
}
