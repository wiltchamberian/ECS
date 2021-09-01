#include "Archetype.h"
#include "ECSMgr.h"

namespace Sun {

	Chunk* ArchetypeBase::applyChunk() {
		return ecsMgr_->memoryPool_->mallocChunk();
	}

	void ArchetypeBase::freeChunk(Chunk* chunk) {
		return ecsMgr_->memoryPool_->freeChunk(chunk);
	}

	EntityAddress ArchetypeBase::requestEntry() {
		EntityAddress res;
		Chunk* chunk = nullptr;
		//只有可能最后一个chunk不满，如果其不满则从它分配
		if (chunks_.size() > 0 && chunks_.back()->numEntity_ < capacity_) {
			chunk = chunks_.back();
			res.chunkIdx = chunks_.size() - 1;
		}
		//所有chunk都已经满了
		else {
			chunk = applyChunk();
			chunks_.emplace_back(chunk);
			res.chunkIdx = chunks_.size() - 1;
		}
		res.idxInChunk = chunk->numEntity_;
		chunk->numEntity_ += 1;

		numEntity_ += 1;

		return res;
	}
}