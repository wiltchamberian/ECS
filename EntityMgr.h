/*****************************************************************************
* @brief : EntityMgr
* @author : acedtang
* @date : 2021/8/19
* revised : 2022/8/6
* @version : ver 1.1
* @inparam :
* @outparam :
*****************************************************************************/

#ifndef __SUN_ENTITY_MGR_H
#define __SUN_ENTITY_MGR_H

#include <functional>
#include <vector>
#include "Util/Traits/TypeList.hpp"
#include "Util/TTuple.h"
#include "ThirdParty/taskflow/taskflow.hpp"

namespace Sun {
 
#define ECS_TAIL const

	template<class _T>
	using SVector = std::vector<_T>;

	/*****************************Entity*********************************/
	using EntityIndex = std::uint32_t;

	class Entity {
	public:
		EntityIndex index = -1;
		std::uint32_t version = -1;
	};

	using EntityAddress = std::size_t;

	struct EntityInfo {
		EntityAddress addr;
		size_t archIndex = 0;
		std::uint32_t version{ 0 }; // version
	};

	/*****************************Entity-end*****************************/

	/*****************************ArchetypeIter*******************************/
	//TODO: a special rebind, how to generalized?
	template<typename A, template<typename...> typename B>
	struct _ArchetypeIter_Rebind;

	template<template<typename...> class A, class... T, template<typename...> typename B>
	struct _ArchetypeIter_Rebind<A<T...>, B> {
		using type = B<typename SVector<T>::iterator...>;
	};

	template<typename A, template<typename...> typename B>
	using ArchetypeIter_Rebind = typename _ArchetypeIter_Rebind<A, B>::type;

	//Archetype iterator，
	//by this, we can efficiently iterator the entities's components in one Archetype
	template<typename _ComSet>
	class ArchetypeIter
	{
	public:
		template<typename ... _Arg>
		friend class Archetype;

		template<typename _ComSet2>
		friend class ArchetypeIter;

		using _My = ArchetypeIter<_ComSet>;
		static constexpr size_t  _ComTypeNum = TListLength_v<_ComSet>;

		template<typename _Archetype>
		ArchetypeIter(const ArchetypeIter<_Archetype>& iter)
		{
			static_assert(TEqualAsSet_v<_Archetype, _ComSet> == true);
			entity_iter = iter.entity_iter;
			for_each_tuple(iterators, [&iter]<typename _T>(_T & com) {
				com = iter.iterators.get<_T>();
			});
		}

		ArchetypeIter()
		{

		}

		template<typename _ComSet2>
		ArchetypeIter<_ComSet2> reduce() {
			static_assert(TContainList_v<_ComSet2, _ComSet>());
			ArchetypeIter<_ComSet2> ret;
			ret.entity_iter = entity_iter;
			for_each_tuple(ret.iterators, [this]<typename _T>(_T & com) {
				com = iterators.get<_T>();
			});
		}

		template<class _ComType>
		_ComType& get() {
			return *iterators.get< SVector<_ComType>::iterator>();
		}

		_My* operator ->() {
			return this;
		}

		_My& operator++ () {
			++entity_iter;
			for_each_tuple(iterators, []<typename _T>(_T & t) {
				++t;
			});
			return *this;
		}
		_My& operator++ (int) {
			ArchetypeIter tmp = *this;
			++(*this);
			return tmp;
		}
		_My& operator-- () {
			--entity_iter;
			for_each_tuple(iterators, []<typename _T>(_T & t) {
				--t;
			});
			return *this;
		}
		_My operator-- (int) {
			ArchetypeIter tmp = *this;
			--(*this);
			return tmp;
		}

		_My& operator+= (size_t siz) {
			entity_iter += siz;
			for_each_tuple(iterators, []<typename _T>(_T & t) {
				t += siz;
			});
			return *this;
		}

		_My& operator-= (size_t siz) {
			entity_iter -= siz;
			for_each_tuple(iterators, []<typename _T>(_T & t) {
				t -= siz;
			});
			return *this;
		}

		_My operator+ (size_t siz) const {
			_My my = *this;
			my += siz;
			return my;
		}

		_My operator- (size_t siz) const {
			_My my = *this;
			my -= siz;
			return my;
		}

		template<typename __Archetype>
		friend inline bool operator == (const ArchetypeIter<__Archetype>& my, const  ArchetypeIter<__Archetype>& other);

		template<typename __Archetype>
		friend inline bool operator != (const ArchetypeIter<__Archetype>& my, const  ArchetypeIter<__Archetype>& other);
	protected:
		SVector<EntityIndex>::iterator entity_iter;
		ArchetypeIter_Rebind<_ComSet, TTuple> iterators;
	};

	template<typename __Archetype>
	inline bool operator == (const ArchetypeIter<__Archetype>& my, const  ArchetypeIter<__Archetype>& other) {
		//a simplied check but works
		return (my.entity_iter == other.entity_iter);
	}

	template<typename __Archetype>
	inline bool operator != (const ArchetypeIter<__Archetype>& my, const  ArchetypeIter<__Archetype>& other) {
		return  (my.entity_iter != other.entity_iter);
	}
	/****************************ArchetypeIter--end************************************/

	/****************************Archetype********************************/
	//非模板形式存储Archetype一些与模板无关的成员与函数
	class ArchetypeBase {
	public:
		friend class EntityMgrBase;

		size_t getEntityNum() const {
			return entities_.size();
		}
		size_t getCapacity() const {
			return entities_.capacity();
		}

	protected:
		SVector<EntityIndex> entities_;

	};

	//forward declare
	template<typename ... _Args>
	class Archetype;

	template<typename _ComSet>
	class ArchetypeIter;

	template<typename ... _Args>
	class Archetype : public ArchetypeBase {
	public:
		template<typename _T>
		friend class ArchetypeIter;

		template<typename ... _Archetypes>
		friend class EntityMgr;

		using ComTypeList = TUnique_t<TypeList<_Args...>>;
		using _My = Archetype<_Args...>;
		static constexpr size_t ComTypeNum = TListLength_v<ComTypeList>;

		static_assert(TIsUnique_v<TypeList<_Args...>> == true, "component type should be unique!");

		Archetype() {}

		~Archetype() {
			//clearAllEntities();
		} 

		template<class _ComType>
		_ComType& getComponent(const EntityAddress& entity) {
			return coms_.get<SVector<_ComType>>()[entity.addr];
		}

		template<class _ComType>
		constexpr bool hasComponent() const {
			return TContain<_ComType, _Args...>::value;
		}

		void clear() {
			entities_.clear();
			for_each_tuple(coms_, []<typename _T>(_T& com) {
				com.clear();
			});
		}

		size_t getEntityNum() const {
			return entities_.size();
		}

		ArchetypeIter<TypeList< _Args...>> begin() {
			Sun::ArchetypeIter<Sun::TypeList< _Args...>> res;
			res.entity_iter = entities_.begin();
			for_each_tuple(coms_, [&res]<typename _T>(_T & com) {
				res.iterators.get<_T::iterator>() = com.begin();
			});
			return res;
		}

		ArchetypeIter<TypeList< _Args...>> end() {
			Sun::ArchetypeIter<Sun::TypeList< _Args...>> res;
			res.entity_iter = entities_.end();
			for_each_tuple(coms_, [&res]<typename _T>(_T & com) {
				res.iterators.get<_T::iterator>() = com.end();
			});
			return res;
		}

	protected:
		TTuple<SVector<_Args>...> coms_;
	};
	/****************************Archetype-end*************************/

	/************************Task************************/
	struct Task {
		ArchetypeBase* arch = nullptr;
		EntityAddress startAddr;
		EntityAddress endAddr;
		std::function<void()> func;
		void operator()() {
			func();
		}
	};
	/************************Task--end***********************/

	/*************************EntityMgr*************************/
	template<typename ... _Archetyeps>
	class EntityMgr;

	class EntityMgrBase {
	public:
		template<typename ... _Arg>
		friend class Archetype;
		EntityMgrBase() {
		}
		~EntityMgrBase() {
		}
		friend class ArchetypeBase;
	
		void runAllTasks() {
			for (auto& it : packages_) {
				//作用于相同chunk的task,添加依赖关系
				tf::Task pre = taskFlow_.emplace(it);
				//for (size_t i = 1; i < it.second.size() - 1; ++i) {
				//	tf::Task t = taskFlow_.emplace(it.second[i]);
				//	pre.precede(t);
				//	pre = t;
				//}
			}
			executor_.run(taskFlow_).wait();
			taskFlow_.clear();
		}

	protected:
		void addTask(const Task& task) {
			packages_.push_back(task);
		}

		template <typename C, std::enable_if_t<tf::is_condition_task_v<C>, void>*>
		tf::Task emplaceTask(C&& task) {
			return taskFlow_.emplace(task);
		}
	protected:
		tf::Taskflow taskFlow_;
		tf::Executor executor_;
		SVector<Task> packages_;
		size_t numEntity_ = 0;

		SVector<EntityInfo> entityTable_;
		EntityIndex freeEntity_ = -1;
	};

	template<typename _List, typename _Archetype>
	struct ArchetypeFilterBinaryOp {
		static constexpr bool value = TContainList<_Archetype, _List>::value;
	};

	template<typename ... _Archetypes>
	class EntityMgr : public EntityMgrBase
	{
	public:
		static_assert(TIsUnique_v<TypeList<_Archetypes...>,TEqualAsSet> == true);
		using ArchetypeTuple = TTuple<_Archetypes...>;
		//using ComList = TypeList<int, float,char, AAA>;//fix me
		using ComList = TUnique_t<typename TMergeMultiList<_Archetypes...>::type>;
		static constexpr size_t comNum = TListLength_v<ComList>;
		static constexpr size_t archetypeNum = TListLength_v<ArchetypeTuple>;

		EntityMgr() {
			//func_remove_entity
			for_each_tuple(archeTypeTuple_, [this]<typename _Arch>(_Arch & base) {
				constexpr size_t index = TListGetIndex_v<TypeList<_Archetypes...>, _Arch>;
				func_remove_entity[index] = [this](const Entity& entity)->void {
					_Arch& arch = archeTypeTuple_.get<_Arch>();

					EntityInfo& info = entityTable_[entity.index];
					if (info.version != entity.version) {
						return;
					}

					EntityIndex moveIndex = arch.entities_.back();
					arch.entities_[info.addr] = arch.entities_.back();
					for_each_tuple(arch.coms_, [&info]<typename _T>(_T & com) {
						com[info.addr] = com.back();
					});
					entityTable_[moveIndex].addr = entity.index;
					entityTable_[entity.index].version = -1;
					entityTable_[entity.index].addr = freeEntity_;
					freeEntity_ = entity.index;

					arch.entities_.pop_back();
					for_each_tuple(arch.coms_, [&info]<typename _T>(_T & com) {
						com.pop_back();
					});

					numEntity_ -= 1;
				};
			});

			//func_get_component,func_has_component
			for_each_tuple(archeTypeTuple_, [this]<typename _Arch>(_Arch & base) {
				for_each_typelist<ComList>([this]<typename _Com>()
				{
					constexpr size_t comIndex = TListGetIndex_v<ComList, _Com>;
					if constexpr (TContain_v<_Arch, _Com>) {
						func_get_component[TListGetIndex_v<TypeList<_Archetypes...>, _Arch>][comIndex] = [this](const Entity& entity)->void* {
							_Arch& arch = archeTypeTuple_.get<_Arch>();
							return (void*)(&(arch.coms_.get<SVector<_Com>>()[entityTable_[entity.index].addr]));
						};
					}
					func_has_component[TListGetIndex_v<TypeList<_Archetypes...>, _Arch>][comIndex] = TContain_v<TypeList<_Archetypes...>, _Com>;
				});
			});

			//func_move_entity
			for_each_tuple(archeTypeTuple_, [this]<typename _Arch>(_Arch & base) {
				for_each_tuple(archeTypeTuple_, [this]<typename _TargetArch>(_TargetArch & target) {
					constexpr size_t index2 = TListGetIndex_v<TypeList<_Archetypes...>, _TargetArch>;
					func_move_entity[TListGetIndex_v<TypeList<_Archetypes...>, _Arch>][index2] = [this](const Entity& entity)->void {
						if constexpr (std::is_same_v<_Arch, _TargetArch>) {
							return;
						}
						if (!exist(entity)) {
							return;
						}
						EntityAddress addr = entityTable_[entity.index].addr;

						_Arch& srcArch = archeTypeTuple_.get<_Arch>();
						_TargetArch& dstArch = archeTypeTuple_.get< _TargetArch>();

						//auto newEnt = createEntity<_TargetArch>();
						for_each_tuple(dstArch.coms_, []<typename _T>(_T & com) {
							com.emplace_back();
						});
						dstArch.entities_.push_back(entity.index);

						for_each_typelist<_TargetArch>([&srcArch, &dstArch,&entity,this]<typename _T>() {
							if constexpr (TContain_v< _Arch, _T>) {
								dstArch.coms_.get<SVector<_T>>().back() = srcArch.coms_.get<SVector<_T>>()[entityTable_[entity.index].addr];
							}
						});

						EntityIndex lastIndex = srcArch.entities_.back();
						for_each_tuple(srcArch.coms_, [addr]<typename _T>(_T & com) {
							com[addr] = com.back();
							com.pop_back();
						});
						srcArch.entities_.pop_back();

						entityTable_[lastIndex].addr = addr;

						entityTable_[entity.index].addr = dstArch.entities_.size() - 1;
						entityTable_[entity.index].archIndex = TListGetIndex_v<ArchetypeTuple, _TargetArch>;

						return;
					};

					
				});
			});
		}
		~EntityMgr() {
		}

		bool exist(const Entity& entity) ECS_TAIL {
			return entity.index >= 0
				&& entity.index < entityTable_.size()
				&& entity.version == entityTable_[entity.index].version
				&& (entityTable_[entity.index].addr != -1)
				/*&& entities_[entityTable_[entity.index].addr] == entity.index*/;
		}

		template<typename _Archetype>
		_Archetype& getArchetype() {
			static_assert(TContain_v< ArchetypeTuple, _Archetype> == true);
			return archeTypeTuple_.get<_Archetype>();
		}

		template<typename _Archetype>
		const _Archetype& getArchetype() const {
			static_assert(TContain_v< ArchetypeTuple, _Archetype> == true);
			return archeTypeTuple_.get<_Archetype>();
		}

		//通过原型创建Entity
		template<typename _Archetype>
		Entity createEntity() {
			_Archetype& archetypeMgr = archeTypeTuple_.get<_Archetype>();

			Entity entity;
			if (freeEntity_ == -1) {
				std::size_t index = entityTable_.size();
				entityTable_.emplace_back();
				entity.index = index;
				entity.version = entityTable_[index].version;
			}
			else {
				entity.index = freeEntity_;
				entity.version = entityTable_[freeEntity_].version;
				freeEntity_ = entityTable_[freeEntity_].addr;
			}

			archetypeMgr.entities_.emplace_back(entity.index);
			for_each_tuple(archetypeMgr.coms_, []<typename _T>(_T & com) {
				com.emplace_back();
			});

			EntityInfo& info = entityTable_[entity.index];
			info.addr = archetypeMgr.entities_.size() - 1;
			info.archIndex = TListGetIndex_v<ArchetypeTuple, _Archetype>;

			numEntity_ += 1;
			return entity;
		}

		//change entity's Archetype
		template<typename _TargetArchetype>
		void moveEntity(const Entity& entity) {
			constexpr size_t index = TListGetIndex_v<ArchetypeTuple, _TargetArchetype>;
			EntityAddress archIndex = entityTable_[entity.index].archIndex;
			func_move_entity[archIndex][index](entity);
		}

		//destroy entity
		void removeEntity(const Entity& entity) {
			if (!exist(entity)) {
				throw std::invalid_argument("Entity is invalid!");
				return;
			}
			size_t archIndex = entityTable_[entity.index].archIndex;
			func_remove_entity[archIndex](entity);
		}

		//after call this, all the exterior reference of entity would be invalid
		void clear() {
			for_each_tuple(archeTypeTuple_, []<typename _T>(_T & arch) {
				arch.clear();
			});
			entityTable_.clear();
			numEntity_ = 0;
			freeEntity_ = -1;
		}

		template<typename _ComType>
		_ComType& getComponent(const Entity& entity) {
			return *(_ComType*)(func_get_component[entityTable_[entity.index].archIndex][TListGetIndex_v<ComList, _ComType>](entity));
		}

		//same as getComponent
		template<typename _ComType>
		_ComType& get(const Entity& entity) {
			return *(_ComType*)(func_get_component[entityTable_[entity.index].archIndex][TListGetIndex_v<ComList, _ComType>](entity));
		}

		template<typename _ComType>
		bool hasComponent(const Entity& entity) ECS_TAIL {
			return func_has_component[entityTable_[entity.index].archIndex][TListGetIndex_v<ComList, _ComType>];
		}

		template<typename _Archetype>
		constexpr bool hasArchetype() ECS_TAIL {
			return TContain_v<ArchetypeTuple, _Archetype>;
		}

		template<typename _Archetype>
		size_t getArchetypeEntityNum() ECS_TAIL {
			static_assert(TContain_v<ArchetypeTuple, _Archetype>);
			return archeTypeTuple_.get<_Archetype>().getEntityNum();
		}

		template<typename _Archetype>
		ArchetypeIter<_Archetype> begin() {
			if (hasArchetype<_Archetype>() == false) {
				return Sun::ArchetypeIter<_Archetype>();
			}
			_Archetype& amgr = archeTypeTuple_.get<_Archetype>();
			return amgr.begin();
		}

		template<typename _Archetype>
		ArchetypeIter<_Archetype> end() {
			if (hasArchetype<_Archetype>() == false) {
				return Sun::ArchetypeIter<_Archetype>();
			}
			_Archetype& amgr = archeTypeTuple_.get<_Archetype>();
			return amgr.end();
		}

		size_t getTotalEntityNum() ECS_TAIL {
			return numEntity_;
		}

		constexpr size_t getArchetypeNum() ECS_TAIL {
			return TListLength_v<ArchetypeTuple>;
		}

		constexpr size_t getComponentNum() ECS_TAIL {
			return comNum;
		}

		template<typename ... _MComType>
		void entity_for_each(std::function<void(_MComType...)> func) {
			constexpr size_t siz = sizeof...(_MComType);

			using FilteredArchetypeTuple = TFilter_t<ArchetypeTuple, TBind1st<ArchetypeFilterBinaryOp, TypeList<std::decay_t<_MComType>...>>::template type_template>;
			static_assert(siz > 0 && TGetLength_v< FilteredArchetypeTuple> > 0);

			for_each_typelist<FilteredArchetypeTuple>([this, func]<typename _Archetype>() {
				auto& archetype = getArchetype<_Archetype>();
				ArchetypeIter<_Archetype> iter = archetype.begin();
				ArchetypeIter<_Archetype> iter2 = archetype.end();
				std::function<void()> wrapperfunc = [iter, iter2, func]() {
					for (auto it = iter; it != iter2;++it) {
						func(it.get<std::decay_t<_MComType>>()...);
					}
				};
				Task task;
				task.startAddr = 0;
				task.endAddr = archetype.getEntityNum();
				task.arch = &archetype;
				task.func = wrapperfunc;
				addTask(task);
			});

		}

	protected:
		ArchetypeTuple archeTypeTuple_;
		
		std::array<std::function<bool(const Entity& en)>, archetypeNum> func_exist;
		std::array<std::function<void(const Entity&)>, archetypeNum> func_remove_entity;
		std::array<std::array<std::function<void* (const Entity&)>, comNum>, archetypeNum> func_get_component;
		std::array<std::array<bool,comNum>,archetypeNum> func_has_component;
		std::array<std::array<std::function<void(const Entity& en)>, archetypeNum>, archetypeNum> func_move_entity;
	};
	/*************************EntityMgr--end*************************/
	
}


#endif