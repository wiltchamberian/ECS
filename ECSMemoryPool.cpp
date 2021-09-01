#include "ECSMemoryPool.h"
#include "Chunk.h"

namespace Sun {

	Chunk* ECSMemoryPool::mallocChunk() {
		Chunk* chunk = new Chunk();
		return chunk;
	}

	void ECSMemoryPool::freeChunk(Chunk* chunk) {
		delete chunk;
	}
}