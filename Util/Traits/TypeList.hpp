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

}

#include "TypeList.inl"
