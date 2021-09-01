/*****************************************************************************
* @brief : Traits
* 提供一些基础模板函数，方便模板操作
* @author : acedtang
* @date : 2021/8/25
* @version : ver 1.0
* @inparam :
* @outparam :
*****************************************************************************/

#ifndef __SUN_TRAITS_H
#define __SUN_TRAITS_H

#include "TypeList.hpp"

namespace Sun {

	/********************获取变长模板参数的长度******************/
	//效果等价于sizeof...(_Args)
	template<typename ... _Args>
	struct TGetLength;

	//特化
	template<>
	struct TGetLength<> {
		static constexpr size_t value = 0;
	};

	template<typename _T, typename ..._Args>
	struct TGetLength<_T,_Args...> {
		static constexpr size_t value = TGetLength<_Args...>::value + 1;
	};

	template<typename ... _Args>
	constexpr size_t TGetLength_v = TGetLength<_Args...>::value;

	//list版本
	template<typename _TypeList,template<typename ...> typename _List>
	struct TListGetLength;
	template<typename _TypeList, template<typename ...> typename _List>
	constexpr size_t TListGetLength_v = TListGetLength<_TypeList, _List>::value;

	template<template<typename ...> typename _List, typename ... _Args>
	struct TListGetLength<_List<_Args...>, _List> {
		static constexpr size_t value = TGetLength_v<_Args...>;
	};


	/*************************获取变长模板参数中某个类型_T的索引**********************/
	template<typename _T , typename ... _Args>
	struct TGetIndex;

	template<typename _T>
	struct TGetIndex<_T> {
		static constexpr size_t value = -1;
	};

	template<typename _T,typename _Arg, typename ... _Args>
	struct TGetIndex<_T, _Arg,_Args...> {
		static constexpr size_t value 
			= std::is_same_v<_T, _Arg> ? 0 : (TGetIndex<_T, _Args...>::value + 1);
	};

	template<typename _T, typename ... _Args>
	constexpr size_t TGetIndex_v = TGetIndex<_T, _Args...>::value;

	//list版本
	template<typename _TypeList, typename _T, template<typename ...> typename _List>
	struct TListGetIndex;
	template<typename _TypeList, typename _T, template<typename ...> typename _List>
	constexpr size_t  TListGetIndex_v = TListGetIndex<_TypeList,_T,_List>::value;

	template<template<typename ...> typename _List, typename _T, typename ... _Args>
	struct TListGetIndex<_List<_Args...>, _T, _List> {
		static constexpr size_t value = TGetIndex_v<_T, _Args...>;
	};

	/*******************************类型标签**************************/
	/*
	* 作用是给某种类型生成一个对应的标签类型,同时可以取回生成该标签的类型
	*/
	template<typename _Type>
	struct TTag {
		using type = _Type;
	};

	/******************************类型映射器************************/
	template<template<typename,typename > typename _TB,typename _Data>
	struct TTypeMap {
		template<typename _A>
		using type_tempalte = _TB<_A,_Data>;
	};

	/*******************************类型派生**************************/
	template<typename _Tag,typename _Data>
	struct TDerived : public _Data{
		using tag = _Tag;
	};

	/******************************类型绑定**********************************/
	/*
	* 绑定一个2元模板类型的第二参数，使其为1元模板类型
	*/
	template<template<typename,typename> typename _ToBindType, typename _Data>
	struct TBind2nd {
		template<typename _Type>
		using template_type = _ToBindType<_Type, _Data>;
	};


	/********************************全序**********************************/
	/*
	* 作用为按照输入类型序列生成一个从小到大的全序，这个全序类可以作为比较运算符进行
	* 其他的模板操作
	*/
	template<typename ... _Type>
	struct TTotalOrder {
		template<typename _A ,typename _B>
		struct TLess {
			static constexpr bool value = TGetIndex<_A, _Type...>::value < TGetIndex<_B, _Type...>::value;
		};
	};

	/********************************包含********************************/
	/*
	* 判断类型_T是否被包含在类型参数包中
	*/
	template<typename _T, typename ... _Args>
	struct Tcontain;
	template<typename _T, typename ... _Args>
	static constexpr bool Tcontain_v = Tcontain<_T, _Args...>::value;
	
	template<typename _T>
	struct Tcontain<_T> {
		static constexpr bool value = false;
	};

	template<typename _T, typename _Type, typename ... _Types>
	struct Tcontain<_T,_Type,_Types...> {
		static constexpr bool value = std::is_same_v<_T, _Type> ? true : Tcontain<_T, _Types...>::value;
	};


	/**********************************去重***********************/
	/*
	* 对输入类型去重之后，返回剩余类型的TypeList
	*/
	template<template<typename...> typename TList, typename ... _Types> //forward declare
	struct TDuplicateRemoval;

	template<template<typename...> typename TList, typename _Type> //recursive terminate
	struct TDuplicateRemoval<TList,_Type> {
		using type = TList<_Type>;
	};

	template<template<typename...> typename TList, typename _Type, typename ... _Types>
	struct TDuplicateRemoval<TList,_Type,_Types...> {
		using type
			= std::conditional_t<TContain<TList<_Types...>,_Type>::value,
			typename TDuplicateRemoval<TList, _Types...>::type,
			TPushFront_t<typename TDuplicateRemoval<TList, _Types...>::type, _Type>>;
	};
	
		
	/*****************************从多个泛型List中提取出所有不重复类型的泛型List*************/
	template<typename ... _TypeLists>
	struct TUnion{};
	template<typename ... _TypeLists>
	using TUnion_t = typename TUnion<_TypeLists...>::type;

	template<typename _TypeList>
	struct TUnion<_TypeList>{
		using type = typename TUnique<_TypeList>::type;
	};

	template<typename _TypeList1,typename _TypeList2>
	struct TUnion<_TypeList1, _TypeList2> {

	};

	template<template<typename...> typename TList, typename _TypeList1>
	struct TUnion<_TypeList1, TList<>> {
		using type = typename TUnion<_TypeList1>::type;
	};

	template<template<typename...> typename TList, typename _TypeList1, typename _Type, typename ... _Types>
	struct TUnion<_TypeList1, TList<_Type, _Types...>> {
		using type = typename TUnion<TUnique_t<TPushBack_t<_TypeList1, _Type>>, TList<_Types...>>::type;
	};

	template<typename _TypeList, typename ... _TypeLists>
	struct TUnion<_TypeList, _TypeLists...> {
		using type = typename TUnion <typename TUnion <_TypeList > ::type, typename TUnion<_TypeLists...>::type > ::type;
	};

	/*****************************把TypeList转为对应的tuple*************/
	template<typename _TypeList>
	struct TTypeListToTuple;

	template<typename _TypeList>
	using TTypeListToTuple_t = typename TTypeListToTuple<_TypeList>::type;

	template<typename ... _Args>
	struct TTypeListToTuple<TypeList<_Args...>> {
		using type = std::tuple<_Args...>;
	};

	/******************************集合减法*****************************/
	template<typename _TypeList1,typename _TypeList2>
	struct TSubAsSet;
	template<typename _TypeList1, typename _TypeList2>
	using TSubAsSet_t = typename TSubAsSet<_TypeList1, _TypeList2>::type;
	template<template<typename...> typename _List2, typename _TypeList1>
	struct TSubAsSet<_TypeList1, _List2<>> {
		using type = _TypeList1;
	};
	template<template<typename...> typename _List2, typename _TypeList1, typename _Head,typename ..._Tail>
	struct TSubAsSet<_TypeList1, _List2<_Head, _Tail...>> {
		using type = typename TSubAsSet<TErase_t<_TypeList1, _Head>, _List2<_Tail...>>::type;
	};

	/***********************************构造，拷贝构造，移动，赋值，析构函数******************/
	template<typename _ComType>
	void construct(void* addr) {
		new (addr)_ComType();
	}
	template<typename _ComType>
	void copy_construct(void* dst,const void* src) {
		new (dst) _ComType(*static_cast<const _ComType*>(src));
	}
	template<typename _ComType>
	void move_construct(void* dst, void* src) {
		new(dst)_ComType(std::move(*static_cast<const _ComType*>(src)));
	}
	template<typename _ComType>
	void move_assign(void* dst, void* src) {
		*static_cast<_ComType*>(dst) = std::move(*static_cast<_ComType*>(src));
	}
	template<typename _ComType>
	void destruct(void* addr) {
		static_cast<_ComType*>(addr)->~_ComType();
	}

	/******************************类似tuple的能力***************/
	template<typename ...Args>
	struct Tuple;

	template<typename _Arg>
	struct Tuple<_Arg> {
		_Arg arg_;
	};

	template<typename _Arg,typename ... _Args>
	struct Tuple<_Arg,_Args...> : public Tuple<_Args...>{
		_Arg arg_;
	};

	//静态注册,将_T类型注册到_DynamicTuple中的到新类型
	template<typename _T, typename _Tuple>
	struct static_regist {
	};
	template<typename _T ,typename ..._Args>
	struct static_regist<_T, Tuple<_Args...>> {
		using type = Tuple<_T, _Args...>;
	};

	//生成某个过滤器模板的反向过滤器模板
	template<template<typename> typename Filter, typename _T>
	struct _ReverseFilter {
		static constexpr bool value = !Filter<_T>::value;
	};
	template<template<typename> typename Filter>
	struct TFilterReverse {
		template<typename _T>
		using type_template = _ReverseFilter <Filter, _T>;
	};

	//根据模板List生成一个过滤器，位于List中的类型通过过滤，反之不通过
	template<typename List, typename _T>
	struct _ListFilter {
		static constexpr bool value = TContain_v<List, _T>;
	};
	template<typename List>
	struct TFilterGenerator {
		template<typename _T>
		using type_template = _ListFilter<List, _T>;
	};

	
	template<typename _T>
	struct TAllPassFilter {
		static constexpr bool value = true;
	};

	template<typename _T>
	using TDefaultFilter = TAllPassFilter<_T>;

	//实现tuple的循环遍历
	template <template<typename> typename Filter, typename Functor ,typename _Ele>
	constexpr void for_each_tuple_impl2(Functor&& f, _Ele& t)
	{
		if constexpr (Filter<_Ele>::value == true) {
			f(t);
		}
	}
	template <template<typename> typename Filter, typename Functor, typename Tuple, std::size_t... Is>
	constexpr void for_each_tuple_impl(Functor&& f, Tuple& t, std::index_sequence<Is...>)
	{
		(for_each_tuple_impl2<Filter,Functor>(std::forward<Functor>(f),std::get<Is>(t)), ...);
	}
	template <template<typename> typename Filter, typename Functor,typename Tuple>
	constexpr void for_each_tuple(Functor&& f,Tuple& t)
	{
		for_each_tuple_impl<Filter, Functor,Tuple>(std::forward<Functor>(f), t, std::make_index_sequence<std::tuple_size_v<Tuple>>());
	}
	template <typename Functor, typename Tuple>
	constexpr void for_each_tuple(Functor&& f, Tuple& t)
	{
		for_each_tuple<TDefaultFilter, Functor, Tuple>(std::forward<Functor>(f), t);
	}

	//获取组件的字节对齐信息
	template<typename _ComType>
	struct TAlignSiz {
		static constexpr size_t value = sizeof(_ComType);
	};
	template<typename _ComType>
	static constexpr size_t TAlignSiz_v = TAlignSiz<_ComType>::value;

	/*******************对一个TypeList的元素进行遍历操作**********************/
	template <template<typename> typename Filter, typename Functor, typename Ele>
	constexpr void for_each_typelist_impl2(Functor&& f)
	{
		if constexpr (Filter<Ele>::value == true) {
			f.operator()<Ele>();
		}
	}

	template <template<typename> typename Filter, typename List, typename Functor, std::size_t... Is>
	constexpr void for_each_typelist_impl(Functor&& f, std::index_sequence<Is...>)
	{
		(for_each_typelist_impl2<Filter, Functor, TAt_t<List,Is>>(std::forward<Functor>(f)), ...);
	}

	template <template<typename> typename Filter, typename List, typename Functor>
	constexpr void for_each_typelist(Functor&& f)
	{
		for_each_typelist_impl<Filter, List, Functor>(std::forward<Functor>(f), std::make_index_sequence<TLength_v<List>>());
	}

	template <typename List, typename Functor>
	constexpr void for_each_typelist(Functor&& f)
	{
		for_each_typelist<TDefaultFilter, List, Functor>(std::forward<Functor>(f));
	}
}


#endif