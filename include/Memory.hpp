#ifndef __MEMORY_HPP__
#define __MEMORY_HPP__

#include <string>
#include <memory>
#include <cmath>
#include <map>
/*
class Chunk {
	private:
	public:
};

struct ChunkInfo {
	size_t size;
	size_t offset;
};

class Memory {
	private:
		std::map<std::string, struct ChunkInfo> m_Chunks;
		size_t	m_NextChunkOffset,
				m_MemoryRegionSize;
		char *m_MemoryRegion;

		template<typename AllocatedType> size_t objectOffset(void)
		{
			size_t typeSize = sizeof(AllocatedType);
			
			if(typeSize % 2)
			{
				return (size_t)(ceil((float)typeSize/16.0f)*16);
			}
			else
			{
				return typeSize;
			}
		}

	public:
		Memory(size_t memoryRegionSize);	// default: 2048 [B]
		virtual ~Memory();

		bool CreateChunk(const std::string& chunkName, size_t chunkSize);
		template<typename AllocatedType> AllocatedType* Allocate(const std::string& chunkName)
		{
			AllocatedType* ptr = new (m_MemoryRegion + m_NextChunkOffset) AllocatedType;
			unsigned int objectSize = objectOffset<AllocatedType>();
			m_NextChunkOffset += objectSize;
			m_Chunks[chunkName] 

			return nullptr;
		}
		bool RemoveChunk(const std::string& chunkName);
};
*/

#endif // __MEMORY_HPP__