#ifndef __MESH_HPP__
#define __MESH_HPP__

#include "Material.hpp"

class Mesh {
	friend class Scene;

	private:
		unsigned int indexCount;
		unsigned int indexOffset;
		Material* material;
		unsigned int indexBuffer;

	public:
		Mesh();
};

#endif // __MESH_HPP__