#include "ECSMgr.h"
#include <stdexcept>

#if (ECS_STATIC_MODE ==1)
	template<typename ... _Archetypes>
	typename Sun::ECSMgr<_Archetypes...>::ArchetypeTuple Sun::ECSMgr<_Archetypes...>::archeTypeTuple_;
#endif

template<typename ... _Archetypes>
size_t Sun::ECSMgr<_Archetypes...>::getTotalEntityNum() ECS_TAIL {
	//return entityTable_.size() - freeEntries_.size();
	GetTotalEntityNum func;
	for_each_tuple(func, archeTypeTuple_);
	return func.siz_;
}

template<typename ... _Archetypes>
constexpr size_t Sun::ECSMgr<_Archetypes...>::getArchetypeNum() ECS_TAIL {
	return std::tuple_size<ArchetypeTuple>::value;
}

template<typename ... _Archetypes>
template<typename _Archetype>
constexpr bool Sun::ECSMgr<_Archetypes...>::hasArchetype() ECS_TAIL {
	return TContain_v<ArchetypeTuple, _Archetype>;
}

template<typename ... _Archetypes>
template<typename _Archetype>
Sun::Entity Sun::ECSMgr<_Archetypes...>::requestFreeEntity() {
	return getArchetype<_Archetype>().requestFreeEntity();
}

//通过原型创建Entity
template<typename ... _Archetypes>
template<typename _Archetype>
Sun::TEntity<_Archetype> Sun::ECSMgr<_Archetypes...>::createEntity() {
	_Archetype& archetypeMgr = std::get<_Archetype>(archeTypeTuple_);
	return TEntity<_Archetype>(archetypeMgr.createEntity());
}

//以entity为原型创建entity
template<typename ... _Archetypes>
template<typename TargetArchetype, typename _Archetype>
ECS_HEAD Sun::TEntity<TargetArchetype> Sun::ECSMgr<_Archetypes...>::createEntity(const Sun::TEntity<_Archetype>& entity) {
	static_assert(TContain_v<ArchetypeTuple, TargetArchetype> == true);
	if (!exist(entity)) {
		return Sun::TEntity<TargetArchetype>();
	}
	_Archetype& srcMgr = std::get<_Archetype>(archeTypeTuple_);
	TargetArchetype& dstMgr = std::get<TargetArchetype>(archeTypeTuple_);

	//申请新entity的内存区域
	EntityAddress dstAddr = dstMgr.requestEntry();
	EntityAddress srcAddr = srcMgr.getEntityTable()[entity.index];
	//申请新entity节点
	Entity newEntity = dstMgr.requestFreeEntity();
	//正向映射
	dstMgr.getEntityTable()[newEntity.index].chunkIdx = dstAddr.chunkIdx;
	dstMgr.getEntityTable()[newEntity.index].idxInChunk = dstAddr.idxInChunk;
	//反向映射
	Entity* dst = dstMgr.getEntity(dstAddr);
	*dst = newEntity;

	//组件拷贝
	ComTypeMove<_Archetype, TargetArchetype> func{ srcMgr, dstMgr,srcAddr,dstAddr };
	for_each_typelist<TargetArchetype>(func);

	return TEntity< TargetArchetype>(newEntity);
}

//移动entity,将一种entity转为为另一种的方法（该方法等价于addComponents或者removeComponents)
template<typename ... _Archetypes>
template<typename TargetArchetype, typename _Archetype>
ECS_HEAD Sun::TEntity<TargetArchetype> Sun::ECSMgr<_Archetypes...>::createEntity(TEntity<_Archetype>&& entity) {
	static_assert(TContain_v<ArchetypeTuple,TargetArchetype> == true);
	if (!exist(entity)) {
		return Sun::TEntity<TargetArchetype>();
	}
	TargetArchetype& dstMgr = std::get<TargetArchetype>(archeTypeTuple_);
	_Archetype& srcMgr = std::get<_Archetype>(archeTypeTuple_);
	auto& srcTable = srcMgr.getEntityTable();
	auto& dstTable = dstMgr.getEntityTable();
	if constexpr (TEqualAsSet_v < _Archetype, TargetArchetype > == true) {
		//正向索引
		Entity newEntity = dstMgr.requestFreeEntity();
		dstTable[newEntity.index].chunkIdx = srcTable[entity.index].chunkIdx;
		dstTable[newEntity.index].idxInChunk = srcTable[entity.index].idxInChunk;
		//反向索引
		Entity* ptr = dstMgr.getEntity(newEntity);
		*ptr = newEntity;
		//回收原entity
		srcMgr.recycleEntityEntry(entity);
		entity.index = -1;
		return TEntity< TargetArchetype>(newEntity);
	}

	//申请新entity的内存区域
	EntityInfo& info = srcTable[entity.index];
	EntityAddress dstAddr = dstMgr.requestEntry();
	EntityAddress srcAddr = info;

	//申请新entity节点
	Entity newEntity = dstMgr.requestFreeEntity();

	//新entity正向映射简历
	dstTable[newEntity.index].chunkIdx = dstAddr.chunkIdx;
	dstTable[newEntity.index].idxInChunk = dstAddr.idxInChunk;

	//新entity反向映射建立
	Entity* ptr = dstMgr.getEntity(newEntity);
	*ptr = newEntity;

	//组件拷贝
	ComTypeMove<_Archetype, TargetArchetype> func{ srcMgr, dstMgr,srcAddr,dstAddr };
	for_each_typelist<TargetArchetype>(func);

	//释放原来的entity
	srcMgr.erase(entity);

	entity.index = -1;
	return TEntity< TargetArchetype>(newEntity);
}

//销毁entity
template<typename ... _Archetypes>
template<typename _Archetype>
void Sun::ECSMgr<_Archetypes...>::eraseEntity(const TEntity<_Archetype>& entity) {
	if (!exist(entity)) {
		throw std::invalid_argument("Entity is invalid!");
		return;
	}
	_Archetype& archetypeMgr = std::get<_Archetype>(archeTypeTuple_);
	archetypeMgr.erase(entity);
}

template<typename ... _Archetypes>
void Sun::ECSMgr<_Archetypes...>::clearAllEntities() {
	ClearAll func;
	for_each_tuple(func, archeTypeTuple_);
}

template<typename ... _Archetypes>
template<typename _ComType, typename _Archetype>
_ComType& Sun::ECSMgr<_Archetypes...>::getComponent(const TEntity<_Archetype>& entity) {
	static_assert(TContain_v < _Archetype, _ComType > == true);
	auto& arch = std::get<_Archetype>(archeTypeTuple_);
	if (!arch.exist(entity)) {
		throw std::invalid_argument("Entity is invalid!");
	}
	return arch.getComponent<_ComType>(entity);
}

template<typename ... _Archetypes>
template<typename _ComType, typename _Archetype>
_ComType& Sun::ECSMgr<_Archetypes...>::get(const TEntity<_Archetype>& entity) {
	static_assert(TContain_v < _Archetype, _ComType > == true);
	auto& arch = std::get<_Archetype>(archeTypeTuple_);
	if (!arch.exist(entity)) {
		throw std::invalid_argument("Entity is invalid!");
	}
	return arch.getComponent<_ComType>(entity);
}

template<typename ... _Archetypes>
template<typename _ComType, typename _Archetype>
bool Sun::ECSMgr<_Archetypes...>::hasComponent(const _ComType& cmpt, const Sun::TEntity<_Archetype>& entity) ECS_TAIL {
	auto& arch = std::get<_Archetype>(archeTypeTuple_);
	if (!arch.exist(entity)) {
		return false;
	}
	return arch.hasComponent<_ComType>();
}

template<typename ... _Archetypes>
template<typename _Archetype>
size_t Sun::ECSMgr<_Archetypes...>::getArchetypeEntityNum() ECS_TAIL {
	bool has = hasArchetype<_Archetype>();
	if (!has) return 0;
	return std::get<_Archetype>(archeTypeTuple_).getEntityNum();
}

template<typename ... _Archetypes>
template<typename _Archetype>
Sun::ArchetypeIter<_Archetype> Sun::ECSMgr<_Archetypes...>::begin() {
	if(hasArchetype<_Archetype>() == false) {
		return Sun::ArchetypeIter<_Archetype>();
	}

	_Archetype& amgr = std::get<_Archetype>(archeTypeTuple_);

	ArchetypeIter<_Archetype> iter;

	iter.setChunks(amgr.getChunks().data());
	iter.setCapacity(amgr.getCapacity());
	iter.setOffsets(amgr.getOffsets());
	return iter;
}

template<typename ... _Archetypes>
template<typename _Archetype>
Sun::ArchetypeIter<_Archetype> Sun::ECSMgr<_Archetypes...>::end() {
	if (hasArchetype<_Archetype>()==false) {
		return Sun::ArchetypeIter<_Archetype>();
	}

	_Archetype& amgr = std::get<_Archetype>(archeTypeTuple_);

	ArchetypeIter<_Archetype> iter;

	iter.setChunks(amgr.getChunks().data());
	iter.setCapacity(amgr.getCapacity());
	iter.setOffsets(amgr.getOffsets());
	iter.setOffset(amgr.getEntityNum());
	return iter;
}

template<typename ... _Archetypes>
template<typename ... _MComType>
void Sun::ECSMgr<_Archetypes...>::entity_for_each(std::function<void(_MComType...)> func) {
	constexpr size_t siz = sizeof...(_MComType);

	using FilteredArchetyepTuple = TFilter_t<ArchetypeTuple, TBind1st<ArchetypeFilterBinaryOp, TypeList<std::decay_t<_MComType>...>>::template type_template>;
	static_assert(siz > 0 && std::tuple_size_v< FilteredArchetyepTuple> > 0);
	
	using MyFunctor = Functor<_Archetypes...>;

	typename MyFunctor::template FunctorInternal<_MComType...> functor;
	functor.mgr_ = this;
	functor.func_ = func;

	for_each_typelist<FilteredArchetyepTuple>(functor);

	/*TFilterGenerator< FilteredArchetyepTuple>::template type_template*/
	//for_each_tuple<TFilterGenerator< FilteredArchetyepTuple>::template type_template>(func,functor.)
	
}

template<typename ... _Archetypes>
typename Sun::ECSMgr<_Archetypes...>::ArchetypeTuple& Sun::ECSMgr<_Archetypes...>::getArchetypeTuple() {
	return archeTypeTuple_;
}

struct Validate {
	template<typename _Archetype>
	void operator()(_Archetype& arch) {
		allValid = allValid && arch.checkValidate();
	}
	bool allValid = true;
};

template<typename ... _Archetypes>
bool Sun::ECSMgr<_Archetypes...>::checkValidate() const {
	Validate func;
	for_each_tuple(func, archeTypeTuple_);
	return func.allValid;
}