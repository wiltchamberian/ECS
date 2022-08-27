#pragma once

#include <cstddef>
#include <functional>

//TypeList所有函数已经完全解析完毕并加了注释
namespace Sun {

	template<typename ...>
	struct TTypeList{};

	template<template<typename ... > typename _ToBindType, typename _Data>
	struct TBind1st {
		template<typename ... _Args>
		using type_template = _ToBindType<_Data, _Args...>;
	};

	//specail case process ,not general
	template<template<template<typename...> typename ,typename,typename> typename _ToBindType, template<typename>typename _Data>
	struct TBind1st3 {
		template<typename _A, typename _B>
		using template_type = _ToBindType<_Data,_A,_B>;
	};

	//从列表实例化后的模板类 获得其模板类型
	template<typename _Class>
	struct TTemplateType {
		template<typename ..._T>
		using type_template = TTypeList<_T...>;
	};
	template<template<typename...> typename _Template, typename ... _T>
	struct TTemplateType<_Template<_T...>> {
		template<typename ... _T>
		using type_template = _Template<_T...>;
	};

	template<typename... Ts>
	struct TypeList {};

	//for test
	template<typename ... Ts>
	struct TypeList2 {};

	template<template<typename...> typename T, typename... Ts>
	struct is_instantiable;
	template<template<typename...> typename T, typename... Ts>
	constexpr bool is_instantiable_v = is_instantiable<T, Ts...>::value;

	//判断两个模板是否是同类型模板
	template<template<typename...> class TA, template<typename...> class TB>
	struct is_same_typename_template;
	template<template<typename...> class TA, template<typename...> class TB>
	constexpr bool is_same_typename_template_v = is_same_typename_template<TA, TB>::value;

	//is_instance_of判断第一个类型Instance 是否 是_T<typename ..._Args>类型
	template<typename Instance, template<typename...> class T>
	struct is_instance_of;
	template<typename Instance, template<typename...> class T>
	constexpr bool is_instance_of_v = is_instance_of<Instance, T>::value;

	//把模板OtherListTemplate模板实例类型 转为 TyList类型
	template<template<typename...>class OtherListTemplate, typename OtherList>
	struct ToTypeList;
	template<template<typename...>class OtherListTemplate, typename OtherList>
	using ToTypeList_t = typename ToTypeList<OtherListTemplate, OtherList>::type;

	//把TypeList实例类型转为OtherListTemplate实例类型
	template<typename List, template<typename...>class OtherListTemplate>
	struct ToOtherList;
	template<typename List, template<typename...>class OtherListTemplate>
	using ToOtherList_t = typename ToOtherList<List, OtherListTemplate>::type;

	//判断一个模板是否是TypeList模板
	template<typename List> struct IsTypeList;
	template<typename List> constexpr bool IsTypeList_v = IsTypeList<List>::value;

	/****************revise start here!!!****************************/
	//该模板计算一个TypeList包含的类型数 
	template< typename List> 
	struct TLength;
	template<typename List> 
	constexpr std::size_t TLength_v = TLength<List>::value;

	//计算一个TypeList是否为空,当前实现依赖Length_v
	template<typename List> 
	struct TIsEmpty;
	template<typename List> 
	constexpr bool TIsEmpty_v = TIsEmpty<List>::value;

	//返回TypeList第一个类型
	template<typename List> 
	struct TFront;
	template<typename List> 
	using TFront_t = typename TFront<List>::type;

	//类似indexof，取出TypeList的第N个类型
	template<typename List, std::size_t N> 
	struct TAt;
	template<typename List, std::size_t N> 
	using TAt_t = typename TAt<List, N>::type;

	//取出索引处对应的类型形成TypeList
	template<typename List, std::size_t... Indices> 
	struct TSelect;
	template<typename List, std::size_t... Indices>
	using TSelect_t = typename TSelect<List, Indices...>::type;

	constexpr std::size_t Find_fail = static_cast<std::size_t>(-1);

	//Find的作用是找出TypeList中T类型第index+1次出现的索引，如果没有出线返回Find_fail
	template<typename List, typename T ,template<typename,typename> typename Equal = std::is_same> 
	struct TFind;
	template<typename List, typename T, template<typename, typename> typename Equal = std::is_same>
	constexpr std::size_t TFind_v = TFind<List, T ,Equal>::value;

	//Find的实现有点复杂，这里扩展为找到_T类型第index+1次出现的索引
	template<typename List, typename _T, size_t index = 0>
	struct TIndex;
	template<typename List, typename _T, size_t index = 0>
	constexpr std::size_t TIndex_v = TIndex<List, _T,index >::value;
	template<template<typename ...> typename TList,typename _T, size_t index>
	struct TIndex<TList<>, _T, index> {
		static constexpr size_t value = Find_fail;
	};
	template<template<typename ...> typename TList, typename _Head, typename ..._Tail, typename _T>
	struct TIndex<TList<_Head, _Tail...>, _T, 0> {
		static constexpr size_t value = std::is_same_v<_T,_Head>?
			0: TIndex<TList<_Tail...>, _T, 0>::value + 1;
	};
	template<template<typename ...> typename TList, typename _Head,typename ..._Tail,typename _T,size_t index>
	struct TIndex<TList<_Head,_Tail...>,_T,index> {
		static constexpr size_t value = 
			std::is_same_v<_T, _Head>?
			TIndex<TList<_Tail...>, _T, index - 1>::value + 1:
			TIndex<TList<_Tail...>, _T, index>::value + 1;
	};

	//找到TypeList中满足Op<Type>的第一个Type出现的索引
	template<typename List, template<typename>class Op> 
	struct TFindIf;
	template<typename List, template<typename>class Op> 
	constexpr std::size_t TFindIf_v = TFindIf<List, Op>::value;

	//利用Find,返回TypeList是否包含T类型
	template<typename List, typename T ,template<typename,typename> typename Equal = std::is_same>
	struct TContain;
	template<typename List, typename T, template<typename, typename> typename Equal = std::is_same>
	constexpr bool TContain_v = TContain<List, T, Equal>::value;

	//返回是否包含...Ts中的所有类型
	template<typename List, typename... Ts> 
	struct TContainTs;
	template<typename List, typename... Ts> 
	constexpr bool TContainTs_v = TContainTs<List, Ts...>::value;

	//返回TypeList0是否包含TypeList1中的所有类型
	template<typename List0, typename List1>
	struct TContainList;
	template<typename List0, typename List1>
	constexpr bool TContainList_v = TContainList<List0, List1>::value;

	//设List的参数为...Args,返回T<Args...>是否合法
	template<typename List, template<typename...> class T>
	struct TCanInstantiate;
	template<typename List, template<typename...> class T>
	constexpr bool TCanInstantiate_v = TCanInstantiate<List, T>::value;

	//设List的参数为...Args, 对应T<Args...>类型
	template<typename List, template<typename...> class T>
	struct TInstantiate;
	template<typename List, template<typename...> class T>
	using TInstantiate_t = typename TInstantiate<List, T>::type;

	//返回T能否实例化List中的某个类型 （为何T需要复数参数?)
	template<typename List, template<typename...>class T>
	struct TExistInstance;
	template<typename List, template<typename...>class T>
	constexpr bool TExistInstance_v = TExistInstance<List, T>::value;

	// get first template instantiable type
	//返回List中可以被T实例化的第一个类型
	template<typename List, template<typename...>class T>
	struct TSearchInstance;
	template<typename List, template<typename...>class T>
	using TSearchInstance_t = typename TSearchInstance<List, T>::type;

	//在TypeList之前push_front上_T类型所得的TypeList类型
	template<typename List, typename T> 
	struct TPushFront;
	template<typename List, typename T> 
	using TPushFront_t = typename TPushFront<List, T>::type;

	//在TypeList之后push_back上_T类型所得的TypeList类型
	template<typename List, typename T> 
	struct TPushBack;
	template<typename List, typename T> 
	using TPushBack_t = typename TPushBack<List, T>::type;

	//表示TypeList的第一个元素popfront之后剩下的TypeList类型
	template<typename List> 
	struct TPopFront;
	template<typename List> 
	using TPopFront_t = typename TPopFront<List>::type;

	//应该是把首元素移动到末尾的TypeList
	template<typename List> 
	struct TRotate;
	template<typename List> 
	using TRotate_t = typename TRotate<List>::type;

	//将List中与_T相同的类型删除返回剩余List
	template<typename List,typename _T>
	struct TErase;
	template<typename List,typename _T>
	using TErase_t = typename TErase<List,_T>::type;
	
	//去重
	template<typename List> 
	struct TUnique;
	template<typename List> 
	using TUnique_t = typename TUnique<List>::type;
	 
	//这个模板比较神奇，其Op是把两个类型映射为一个类型的模板类型,该模板，输入为I,
	//Op作用于I和TypeList的第一个类型得到一个类型T，接着Op再输入T和List的第二个类型做映射得到T1,
	//这样递归下去直到List为空，输出最后的到的类型 ,Op的作用就好像+法在序列中的作用一样
	template<typename List, template <typename I, typename X> class Op, typename I>
	struct TAccumulate;
	template<typename List, template <typename I, typename X> class Op, typename I>
	using TAccumulate_t = typename TAccumulate< List, Op, I>::type;

	//Filter利用了Accumulate,其作用是过滤出TypeList中 满足Op<_T>::value==true 的类型，输出这些类型的
	//TypeList,（但貌似输出是反序的)
	template<typename List, template<typename>class Op> 
	struct TFilter;
	template<typename List, template<typename>class Op> 
	using TFilter_t = typename TFilter<List, Op>::type;

	//把TypeList反序
	template<typename List> 
	struct TReverse;
	template<typename List> 
	using TReverse_t = typename TReverse<List>::type;

	//把TypeList0和TypeList1拼接起来
	template<typename List0, typename List1> 
	struct TConcat;
	template<typename List0, typename List1> 
	using TConcat_t = typename TConcat<List0, List1>::type;

	//用Op作用于TypeList每个元素得到一个新的TypeList
	template<typename List, template<typename> class Op> 
	struct TTransform;
	template<typename List, template<typename> class Op> 
	using TTransform_t = typename TTransform<List, Op>::type;

	//名符其实的对类型进行快速排序
	template<typename List, template<typename X, typename Y>typename Less>
	struct TQuickSort;
	template<typename List, template<typename X, typename Y>typename Less>
	using TQuickSort_t = typename TQuickSort< List, Less>::type;
	
	//表示TypeList中的每个类型是否唯一
	template<typename List ,template<typename ,typename> typename Equal = std::is_same>
	struct TIsUnique;
	template<typename List, template<typename, typename> typename Equal = std::is_same>
	constexpr bool TIsUnique_v = TIsUnique<List, Equal>::value;

	//比较两个unique的list的是否具有相同的元素
	template<typename List1,typename List2>
	struct TEqualAsSet {
		static_assert(TIsUnique_v<List1>&& TIsUnique_v<List2>);
		static constexpr bool value = TContainList_v<List1, List2> && TContainList_v<List2, List1>;
	};
	template<typename List1, typename List2>
	constexpr bool TEqualAsSet_v = TEqualAsSet<List1, List2>::value;

	//将一个list转化为另外一个List
	template< template<typename ...> typename TList2, typename List1>
	struct ListToList;
	template<template<typename ...> typename TList2, typename List1>
	using ListToList_t = typename ListToList<TList2, List1>::type;

	//多个list合并为一个
	template<typename ... _List>
	struct TMergeMultiList;

	template<typename _List>
	struct TMergeMultiList<_List> {
		using type = _List;
	};

	template<typename _List0, typename ... _List1>
	struct TMergeMultiList<_List0, _List1...> {
		using type = TConcat_t<_List0, typename TMergeMultiList<_List1...>::type>;
	};




	///////////////////

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
	struct TGetLength<_T, _Args...> {
		static constexpr size_t value = TGetLength<_Args...>::value + 1;
	};

	template<typename ... _Args>
	constexpr size_t TGetLength_v = TGetLength<_Args...>::value;

	//list版本
	template<typename _TypeList>
	struct TListLength;

	template<template<typename ...> typename _List, typename ... _Args>
	struct TListLength<_List<_Args...>> {
		static constexpr size_t value = TGetLength_v<_Args...>;
	};

	template<typename _TypeList>
	constexpr size_t TListLength_v = TListLength<_TypeList>::value;


	/*************************获取变长模板参数中某个类型_T的索引**********************/
	template<typename _T, typename ... _Args>
	struct TGetIndex;

	template<typename _T>
	struct TGetIndex<_T> {
		static constexpr size_t value = -1;
	};

	template<typename _T, typename _Arg, typename ... _Args>
	struct TGetIndex<_T, _Arg, _Args...> {
		static constexpr size_t value
			= std::is_same_v<_T, _Arg> ? 0 : (TGetIndex<_T, _Args...>::value + 1);
	};

	template<typename _T, typename ... _Args>
	constexpr size_t TGetIndex_v = TGetIndex<_T, _Args...>::value;

	//list版本
	template<typename _TypeList, typename _T>
	struct TListGetIndex;

	template<template<typename ...> typename _List, typename _T, typename ... _Args>
	struct TListGetIndex<_List<_Args...>, _T> {
		static constexpr size_t value = TGetIndex_v<_T, _Args...>;
	};

	template<typename _TypeList, typename _T>
	constexpr size_t  TListGetIndex_v = TListGetIndex<_TypeList, _T>::value;

	/*******************************类型标签**************************/
	/*
	* 作用是给某种类型生成一个对应的标签类型,同时可以取回生成该标签的类型
	*/
	template<typename _Type>
	struct TTag {
		using type = _Type;
	};

	/******************************类型映射器************************/
	template<template<typename, typename > typename _TB, typename _Data>
	struct TTypeMap {
		template<typename _A>
		using type_tempalte = _TB<_A, _Data>;
	};

	/*******************************类型派生**************************/
	template<typename _Tag, typename _Data>
	struct TDerived : public _Data {
		using tag = _Tag;
	};

	/******************************类型绑定**********************************/
	/*
	* 绑定一个2元模板类型的第二参数，使其为1元模板类型
	*/
	template<template<typename, typename> typename _ToBindType, typename _Data>
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
		template<typename _A, typename _B>
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
	struct Tcontain<_T, _Type, _Types...> {
		static constexpr bool value = std::is_same_v<_T, _Type> ? true : Tcontain<_T, _Types...>::value;
	};


	/**********************************去重***********************/
	/*
	* 对输入类型去重之后，返回剩余类型的TypeList
	*/
	template<template<typename...> typename TList, typename ... _Types> //forward declare
	struct TDuplicateRemoval;

	template<template<typename...> typename TList, typename _Type> //recursive terminate
	struct TDuplicateRemoval<TList, _Type> {
		using type = TList<_Type>;
	};

	template<template<typename...> typename TList, typename _Type, typename ... _Types>
	struct TDuplicateRemoval<TList, _Type, _Types...> {
		using type
			= std::conditional_t<TContain<TList<_Types...>, _Type>::value,
			typename TDuplicateRemoval<TList, _Types...>::type,
			TPushFront_t<typename TDuplicateRemoval<TList, _Types...>::type, _Type>>;
	};


	/*****************************从多个泛型List中提取出所有不重复类型的泛型List*************/
	template<typename ... _TypeLists>
	struct TUnion {};
	template<typename ... _TypeLists>
	using TUnion_t = typename TUnion<_TypeLists...>::type;

	template<typename _TypeList>
	struct TUnion<_TypeList> {
		using type = typename TUnique<_TypeList>::type;
	};

	template<typename _TypeList1, typename _TypeList2>
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
	template<typename _TypeList1, typename _TypeList2>
	struct TSubAsSet;
	template<typename _TypeList1, typename _TypeList2>
	using TSubAsSet_t = typename TSubAsSet<_TypeList1, _TypeList2>::type;
	template<template<typename...> typename _List2, typename _TypeList1>
	struct TSubAsSet<_TypeList1, _List2<>> {
		using type = _TypeList1;
	};
	template<template<typename...> typename _List2, typename _TypeList1, typename _Head, typename ..._Tail>
	struct TSubAsSet<_TypeList1, _List2<_Head, _Tail...>> {
		using type = typename TSubAsSet<TErase_t<_TypeList1, _Head>, _List2<_Tail...>>::type;
	};

	/***********************************构造，拷贝构造，移动，赋值，析构函数******************/
	template<typename _ComType>
	void construct(void* addr) {
		new (addr)_ComType();
	}
	template<typename _ComType>
	void copy_construct(void* dst, const void* src) {
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

	template<typename _Arg, typename ... _Args>
	struct Tuple<_Arg, _Args...> : public Tuple<_Args...> {
		_Arg arg_;
	};

	//静态注册,将_T类型注册到_DynamicTuple中的到新类型
	template<typename _T, typename _Tuple>
	struct static_regist {
	};
	template<typename _T, typename ..._Args>
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
	template <template<typename> typename Filter, typename Functor, typename _Ele>
	constexpr void for_each_tuple_impl2(Functor&& f, _Ele& t)
	{
		if constexpr (Filter<_Ele>::value == true) {
			f.operator() < _Ele > (t);
		}
	}
	template <template<typename> typename Filter, typename Tuple, typename Functor, std::size_t... Is>
	constexpr void for_each_tuple_impl(Tuple& t, Functor&& f, std::index_sequence<Is...>)
	{
		(for_each_tuple_impl2<Filter, Functor>(std::forward<Functor>(f), t.get<Is>()), ...);
	}
	template <template<typename> typename Filter, typename Tuple, typename Functor>
	constexpr void for_each_tuple(Tuple& t, Functor&& f)
	{
		for_each_tuple_impl<Filter, Tuple, Functor>(t, std::forward<Functor>(f), std::make_index_sequence<TListLength_v<Tuple>>());
	}

	template <typename Tuple, typename Functor>
	constexpr void for_each_tuple(Tuple& t, Functor&& f)
	{
		for_each_tuple<TDefaultFilter, Tuple, Functor>(t, std::forward<Functor>(f));
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
			f.operator() < Ele > ();
		}
	}

	template <template<typename> typename Filter, typename List, typename Functor, std::size_t... Is>
	constexpr void for_each_typelist_impl(Functor&& f, std::index_sequence<Is...>)
	{
		(for_each_typelist_impl2<Filter, Functor, TAt_t<List, Is>>(std::forward<Functor>(f)), ...);
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

	//rebind
	template<typename A, template<typename...> typename B>
	struct rebind_;

	template<template<typename...> class A, class... T, template<typename...> typename B>
	struct rebind_<A<T...>, B> {
		using type = B<T...>;
	};

	template<typename A, template<typename...> typename B>
	using rebind = typename rebind_<A, B>::type;
}

#include "TypeList.inl"
