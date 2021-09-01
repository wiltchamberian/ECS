#include "Archetype.h"

template<typename ... _ComType>
Sun::Archetype<_ComType...>::Archetype() {
	ObjSizCaculator func;
	for_each_typelist<ComTypeList>(func);
	objectSize_ = func.obj_siz;
	//将entity的size也考虑进去
	entityAlignSiz_ = TAlignSiz_v<Entity>;
	objectSize_ += entityAlignSiz_;
	if (objectSize_ > 0)
		capacity_ = SUN_CHUNK_SIZE / objectSize_;

	//计算偏移量
	size_t offset = entityAlignSiz_ * capacity_;

	SetOffset funcSet{ offset,capacity_,offsets_ };
	for_each_typelist< ComTypeList>(funcSet);
}

template<typename ... _Args>
Sun::Archetype<_Args...>::~Archetype() {
	ComDestructor func{ nullptr,0,offsets_ };
	for (auto& it : chunks_) {
		func.chunk = it;
		for (size_t i = 0; i < it->numEntity_; ++i) {
			func.index = i;
			for_each_typelist<ComTypeList>(func);
		}
	}
}

template<typename ... _Args>
template<class _ComType>
_ComType& Sun::Archetype<_Args...>::getComponent(const Sun::EntityAddress& addr) {
	Chunk* chunk = chunks_[addr.chunkIdx];
	constexpr size_t offsetIndex = TIndex_v<ComTypeList, _ComType, 0>;
	return *(_ComType*)(chunk->data_ + offsets_[offsetIndex] + addr.idxInChunk * TAlignSiz_v<_ComType>);
}

template<typename ... _Args>
template<class _ComType>
_ComType& Sun::Archetype<_Args...>::getComponent(const Sun::Entity& entity) {
	auto& addr = entityTable_[entity.index];
	Chunk* chunk = chunks_[addr.chunkIdx];
	constexpr size_t offsetIndex = TIndex_v<ComTypeList, _ComType,0>;
	return *(_ComType*)(chunk->data_ + offsets_[offsetIndex] + addr.idxInChunk * TAlignSiz_v<_ComType>);
}

template<typename ..._Args>
template<class _ComType>
constexpr bool Sun::Archetype<_Args...>::hasComponent() const {
	return TContain<_ComType, _Args...>::value;
}

template<typename ..._Args>
size_t Sun::Archetype<_Args...>::erase(const Entity& entity) {
	//表示被移动的entity索引
	size_t res = static_cast<size_t>(-1);

	EntityAddress addr = entityTable_[entity.index];

	Chunk* chunk = chunks_[addr.chunkIdx];
	//在chunk上调用析构函数
	ComDestructor func{ chunk,addr.idxInChunk,offsets_ };
	for_each_typelist<ComTypeList>(func);
	//被移动的chunk
	Chunk* srcChunk = chunks_.back();
	//说明待删除的entity不是 最后一个chunk的最后一个entity ,则需要进行移动操作
	if (addr.chunkIdx != chunks_.size() - 1 || addr.idxInChunk < chunk->numEntity_ - 1) {
		
		ComTypeMovement funcMove{ chunk,srcChunk,addr.idxInChunk,offsets_ };
		for_each_typelist<ComTypeList>(funcMove);
		//entity移动
		void* dst = chunk->data_ + entityOffset_ + addr.idxInChunk * entityAlignSiz_;
		void* src = srcChunk->data_ + entityOffset_ + (srcChunk->numEntity_ - 1) * entityAlignSiz_;
		moveEntity(dst, src);
		//更新索引
		Entity* dstEntity = (Entity*)(dst);
		auto& info = entityTable_[dstEntity->index];
		info.chunkIdx = addr.chunkIdx;
		info.idxInChunk = addr.idxInChunk;

		//赋值被移动的entity的索引
		res = dstEntity->index;
	}
	else {
	}
	--srcChunk->numEntity_;
	if (srcChunk->numEntity_ == 0) {
		freeChunk(srcChunk);
		chunks_.pop_back();
	}

	//原型的entity数减去1
	--numEntity_;

	if (res != -1) {
		entityTable_[res].chunkIdx = addr.chunkIdx;
		entityTable_[res].idxInChunk = addr.idxInChunk;
	}

	recycleEntityEntry(entity);

	//返回被移动的entity索引，如果没有被移动则返回-1
	return res;

}

template<typename ..._Args>
void Sun::Archetype<_Args...>::clearAllEntities() {
	ComDestructor func{nullptr,0,offsets_ };
	for (Chunk* chunk : chunks_) {
		for (size_t idx = 0; idx < chunk->numEntity_; ++idx) {
			func.chunk = chunk; 
			func.index = idx;
			for_each_typelist<ComTypeList>(func);
		}
	}
	for (Chunk* chunk : chunks_) {
		freeChunk(chunk);
	}
	chunks_.clear();
	entityTable_.clear();
	freeEntries_.clear();
	numEntity_ = 0;
}

template<typename ..._Args>
void Sun::Archetype<_Args...>::assignObject(const EntityAddress& addr,const Entity& entity) {
	Chunk* chunk = chunks_[addr.chunkIdx];
	//组件分配
	ComTypeConstructor func{ chunk,addr.idxInChunk,offsets_ };
	for_each_typelist<ComTypeList>(func);
	//Entity分配
	Entity* en = (Entity*)(chunk->data_ + entityOffset_ + addr.idxInChunk * entityAlignSiz_);
	*en = entity;
	//反向映射
	EntityInfo& info = entityTable_[entity.index];
	info.chunkIdx = addr.chunkIdx;
	info.idxInChunk = addr.idxInChunk;
}