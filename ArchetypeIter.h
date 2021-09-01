#ifndef __SUN_ECS_ARCHETYPEITER_H
#define __SUN_ECS_ARCHETYPEITER_H

#include <array>
#include "Entity.h"
#include "Util/Traits/Traits.h"
#include "Chunk.h"

namespace Sun {


	//原型迭代器，通过该迭代器可以高效率的迭代某个原型中的所有entity的所有组件
	template<typename _Archetype>
	class ArchetypeIter
	{
	public:
		using _My = ArchetypeIter<_Archetype>;
		static constexpr size_t  _ComTypeNum = TLength_v<_Archetype>;

		ArchetypeIter(Chunk** chunks, size_t offset, size_t capacity, const std::array<size_t, _ComTypeNum>& arr)
		:chunks_(chunks)
		,offset_(offset)
		,capacity_(capacity)
		, offsets_(arr)
		{

		}
		ArchetypeIter() {

		}
	
		inline bool isValid() {
			return chunks_ != nullptr;
		}

		inline void setChunks(Chunk** chunks) {
			chunks_ = chunks;
		}

		Chunk** const getChunks() const {
			return chunks_;
		}

		inline void setOffsets(const std::array<size_t, _ComTypeNum>& arr) {
			offsets_ = arr;
		}

		inline void setOffset(size_t off) {
			offset_ = off;
		}

		size_t getOffset() const {
			return offset_;
		}

		inline void setCapacity(size_t cap) {
			capacity_ = cap;
		}

		template<class _ComType>
		_ComType& get() {
			size_t chunkIdx = offset_ / capacity_; //TODO:this may optimize to 2^n or by some other strategy
			size_t idxInChunk = offset_ % capacity_;
			constexpr size_t comIndex = TFind_v<_Archetype, _ComType>;
			return *(_ComType*)(chunks_[chunkIdx]->data_ + offsets_[comIndex] + TAlignSiz_v<_ComType> *idxInChunk);
		}

		_My* operator ->() {
			return this;
		}

		_My& operator++ () {
			++offset_;
			return *this;
		}
		_My& operator++ (int) {
			ArchetypeIter tmp = *this;
			++offset_;
			return tmp;
		}
		_My& operator-- () {
			--offset_;
			return *this;
		}
		_My& operator-- (int) {
			ArchetypeIter tmp = *this;
			--offset_;
			return tmp;
		}

		_My& operator+= (size_t siz) {
			offset_ += siz;
			return *this;
		}

		_My& operator-= (size_t siz) {
			offset_ -= siz;
			return *this;
		}

		_My operator+ (size_t siz) const {
			_My my = *this;
			my.offset_ += siz;
			return my;
		}

		_My operator- (size_t siz) const {
			_My my = *this;
			my.offset_ -= siz;
			return my;
		}

	protected:
		std::array<size_t, _ComTypeNum> offsets_;

		//标记目标entity在原型中的总实体偏移量
		size_t offset_ = 0;
		//标记原型的单个chunk的容量
		size_t capacity_ = 0;

		Chunk** chunks_ = nullptr;
	};

	template<typename __Archetype>
	inline bool operator == (const ArchetypeIter<__Archetype>& my, const  ArchetypeIter<__Archetype>& other) {
		return (my.getChunks() == other.getChunks()) && (my.getOffset() == other.getOffset());
	}

	template<typename __Archetype>
	inline bool operator != (const ArchetypeIter<__Archetype>& my, const  ArchetypeIter<__Archetype>& other) {
		return (my.getChunks() != other.getChunks()) || (my.getOffset() != other.getOffset());
	}
}


#endif