#pragma once

#include <cstddef>

#if (_HAS_CXX20==0)
namespace std {
	// STRUCT TEMPLATE type_identity
	template <class _Ty>
	struct type_identity {
		using type = _Ty;
	};
	template <class _Ty>
	using type_identity_t = typename type_identity<_Ty>::type;
}
#endif // _HAS_CXX20

namespace Sun::details {
	template<typename Void, template<typename...> typename T, typename... Ts>
	struct is_instantiable : std::false_type {};
	template<template<typename...> typename T, typename... Ts>
	struct is_instantiable<std::void_t<T<Ts...>>, T, Ts...> : std::true_type {};

	template<typename List, typename T, template<typename, typename> typename Equal, std::size_t N = 0, bool found = false>
	struct TFind;

	template<typename List, template<typename>class Op, std::size_t N = 0, bool found = false>
	struct TFindIf;

	template<typename List, template <typename I, typename X> class Op, typename I, bool = TIsEmpty_v<List>>
	struct TAccumulate;

	template<typename List, template<typename...>class T, bool found = false, bool = TIsEmpty<List>::value>
	struct TExistInstance;

	template<typename List, typename LastT, template<typename...>class T, bool found = false, bool = TIsEmpty<List>::value>
	struct TSearchInstance;

	template<typename List,template<typename,typename> typename Equal, bool haveSame = false>
	struct TIsUnique;
}

namespace Sun {

	//表示T<..._Ts>是否合法
	template<template<typename...> typename T, typename... Ts>
	struct is_instantiable : details::is_instantiable<void, T, Ts...> {};

	template<typename Instance, template<typename...> class T>
	struct is_instance_of : std::false_type {};

	template<typename... Args, template<typename...> class T>
	struct is_instance_of<T<Args...>, T> : std::true_type {};


	template<template<typename...>class OtherListTemplate, typename... Ts>
	struct ToTypeList<OtherListTemplate, OtherListTemplate<Ts...>> : std::type_identity<TypeList<Ts...>> {};

	// =================================================

	template<typename... Ts, template<typename...>class OtherListTemplate>
	struct ToOtherList<TypeList<Ts...>, OtherListTemplate> : std::type_identity<OtherListTemplate<Ts...>> {};

	// =================================================

	template<template<typename ...> typename TList1, template<typename ...> typename TList2, typename ... _Ts1>
	struct ListToList<TList2,TList1<_Ts1...>>{
		using type = TList2<_Ts1...>;
	};

	// =================================================

	//template<template<typename ...> typename _TTuple, template<typename> typename Func, typename _Head,typename ... _Tail>
	//struct TupleIterator<_TTuple,Func, _TTuple<_Head,_Tail...>> {
	//	void operator()(_TTuple<_Head,_Tail...>& tuple, Func& func) {
	//		func(std::get<_Head>(tuple));
	//		TupleIterator<_TTuple, Func, _TTuple<_Tail...>>()(tuple, func);
	//	}
	//};

	// =================================================

	template<typename List>
	struct IsTypeList : is_instance_of<List, TypeList> {};

	// =================================================

	template<template<typename ...> typename TList, typename... Ts>
	struct TLength<TList<Ts...>> : std::integral_constant<std::size_t, sizeof...(Ts)> {};

	// =================================================

	template<typename List>
	struct TIsEmpty : std::integral_constant<bool, TLength_v<List> == 0> {};

	// =================================================

	template<template<typename...> typename _List, typename Head, typename... Tail>
	struct TFront< _List<Head, Tail...>> : std::type_identity<Head> {};

	// =================================================

	template<typename List>
	struct TAt<List, 0> : std::type_identity<TFront_t<List>> {};

	template<typename List, std::size_t N>
	struct TAt : TAt<TPopFront_t<List>, N - 1> {};

	// =================================================

	template<typename List, std::size_t... Indices>
	struct TSelect {
		template<typename ... _T>
		using TTmp = typename TTemplateType<List>::type_template<_T...>;
		using type = typename TTmp<TAt_t<List, Indices>...>;
	};

	// =================================================

	template<typename List, typename T, template<typename, typename> typename Equal>
	struct TFind : details::TFind<List, T, Equal> { };

	template<typename List, template<typename>class Op>
	struct TFindIf : details::TFindIf<List, Op> { };

	// =================================================

	template<typename List, typename T,template<typename,typename> typename Equal>
	struct TContain : std::integral_constant<bool, TFind_v<List, T, Equal> != Find_fail> {};

	// =================================================
	//注意后面的逻辑与，则List必须包含所有...Ts中的类型才返回true
	template<typename List, typename... Ts>
	struct TContainTs : std::integral_constant<bool, (TContain_v<List, Ts> &&...)> {};

	// =================================================

	template<template<typename...> typename TList, typename List, typename... Ts>
	struct TContainList<List, TList<Ts...>> : std::integral_constant<bool, (TContain_v<List, Ts> &&...)> {};

	// =================================================

	template<template<typename...> typename TList, template<typename...> class T, typename... Args>
	struct TCanInstantiate<TList<Args...>, T> : is_instantiable<T, Args...> {};

	// =================================================

	template<template<typename...> typename TList, template<typename...> class T, typename... Args>
	struct TInstantiate<TList<Args...>, T> : std::type_identity<T<Args...>> {};

	// =================================================

	template<typename List, template<typename...>class T>
	struct TExistInstance : details::TExistInstance<List, T> {};

	// =================================================

	template<typename List, template<typename...>class T>
	struct TSearchInstance : details::TSearchInstance<List, void, T> {};

	// =================================================

	template<template<typename...> typename TList, typename T, typename... Ts>
	struct TPushFront<TList<Ts...>, T> : std::type_identity<TList<T, Ts...>> {};

	// =================================================

	template<template<typename...> typename TList, typename T, typename... Ts>
	struct TPushBack<TList<Ts...>, T> : std::type_identity<TList<Ts..., T>> {};

	// =================================================

	template<template<typename...> typename TList, typename Head, typename... Tail>
	struct TPopFront<TList<Head, Tail...>> : std::type_identity<TList<Tail...>> {};

	// =================================================

	template<template<typename...> typename TList, typename Head>
	struct TUnique<TList<Head>> {
		using type = TList<Head>;
	};

	template<template<typename...> typename TList, typename Head, typename ... Tail>
	struct TUnique<TList<Head, Tail... >> {
		using type = std::conditional_t<TContain_v<TList<Tail...>, Head>,
			typename TUnique<TList<Tail...>>::type,
			TPushFront_t<typename TUnique<TList<Tail...>>::type, Head>>;
	};

	// =================================================

	template<template<typename...> typename TList, typename Head, typename... Tail>
	struct TRotate<TList<Head, Tail...>> : std::type_identity<TList<Tail..., Head>> {};

	// =================================================

	//将List中与_T相同的类型删除返回剩余List
	template<template<typename...> typename TList, typename _T>
	struct TErase<TList<>, _T> {
		using type = TList<>;
	};
	template<template<typename...> typename TList, typename _T, typename Head, typename... Tail>
	struct TErase<TList<Head, Tail...>,_T> {
		using type = std::conditional_t<std::is_same_v<_T, Head>,
			typename TErase<TList<Tail...>, _T>::type,
			TPushFront_t<typename TErase<TList<Tail...>, _T>::type, Head>>;
	};

	// =================================================

	template<typename List, template <typename I, typename X> class Op, typename I>
	struct TAccumulate : details::TAccumulate<List, Op, I> {};

	// =================================================

	template<typename List, template<typename>class Test>
	struct TFilter {
	private:
		template<typename I, typename X>
		struct TPushFrontIf : std::conditional<Test<X>::value, TPushFront_t<I, X>, I> {};
	public:
		using type = TAccumulate_t<List, TPushFrontIf, typename TTemplateType<List>::template type_template<>>;
	};

	// =================================================

	template<typename List>
	struct TReverse : TAccumulate<List, TPushFront , typename TTemplateType<List>::type_template<>/*TList<>*/> {};

	// =================================================

	template<typename List, typename T>
	struct TPushBack : TReverse<TPushFront_t<TReverse_t<List>, T>> {};

	// =================================================

	template<typename List0, typename List1>
	struct TConcat : TAccumulate<List1, TPushBack, List0> {};

	// =================================================

	template<template<typename...> typename TList, template<typename T> class Op, typename... Ts>
	struct TTransform<TList<Ts...>, Op> : std::type_identity<TList<typename Op<Ts>::type...>> {};

	// =================================================
	//表示空TypList类型就是它自己
	template<template<typename...> typename TList, template<typename X, typename Y>typename Less>
	struct TQuickSort<TList<>, Less> : std::type_identity<TList<>> {};
	//只有单个元素的TypeList的类型也就是它自己
	template<template<typename...> typename TList, template<typename X, typename Y>typename Less, typename T>
	struct TQuickSort<TList<T>, Less> : std::type_identity<TList<T>> {};
	//神奇代码，基本思路是找到TyeList第一个元素，通过之前的Filter过滤出所有小于它的元素，形成TypeList1;
	//再找到所有比它大的元素形成TypeList2,然后对TypeList1和TypeList2递归的sort最后把Head插入到它们中间
	template<template<typename...> typename TList, template<typename X, typename Y>typename Less, typename Head, typename... Tail>
	struct TQuickSort<TList<Head, Tail...>, Less> {
	private:
		template<typename X>
		using LessThanHead = Less<X, Head>;
		template<typename X>
		using GEThanHead = std::integral_constant<bool, !Less<X, Head>::value>;
		using LessList = TFilter_t<TList<Tail...>, LessThanHead>;
		using GEList = TFilter_t<TList<Tail...>, GEThanHead>;
	public:
		using type = TConcat_t<
			typename TQuickSort<LessList, Less>::type,
			TPushFront_t<typename TQuickSort<GEList, Less>::type, Head>>;
	};

	// =================================================


	template<typename List , template<typename,typename> typename Equal>
	struct TIsUnique : details::TIsUnique<List , Equal> {};


}

namespace Sun::details {
	//Find是找出TypeList中第一个符合T类型的索引；这里模板展开大意就是累加索引，直到符合条件返回索引
	//如果都不满足返回-1
	template<template<typename...> typename TList, typename T, template<typename, typename> typename Equal, std::size_t N, typename... Ts>
	struct TFind<TList<Ts...>, T,Equal, N, true> : std::integral_constant<std::size_t, N - 1> {};
	template<template<typename...> typename TList, typename T, template<typename, typename> typename Equal, std::size_t N>
	struct TFind<TList<>, T, Equal, N, false> : std::integral_constant<std::size_t, Find_fail> {};
	template<template<typename...> typename TList, typename T, template<typename, typename> typename Equal, typename Head, std::size_t N, typename... Tail>
	struct TFind<TList<Head, Tail...>, T, Equal, N, false>
		: TFind<TList<Tail...>, T, Equal, N + 1, Equal<T, Head>::value> {};

	// =================================================

	template<template<typename...> typename TList, template<typename>class Op, std::size_t N, typename... Ts>
	struct TFindIf <TList<Ts...>, Op, N, true > : std::integral_constant<std::size_t, N - 1> {};
	template<template<typename...> typename TList, template<typename>class Op, std::size_t N>
	struct TFindIf<TList<>, Op, N, false> : std::integral_constant<std::size_t, Find_fail> {};
	template<template<typename...> typename TList, template<typename>class Op, typename Head, std::size_t N, typename... Tail>
	struct TFindIf<TList<Head, Tail...>, Op, N, false>
		: TFindIf<TList<Tail...>, Op, N + 1, Op<Head>::value> {};

	// =================================================

	template<typename List, template <typename I, typename X> class Op, typename I>
	struct TAccumulate<List, Op, I, false> : TAccumulate<TPopFront_t<List>, Op, typename Op<I, TFront_t</*TList, */List>>::type> { };

	template<typename List, template <typename X, typename Y> class Op, typename I>
	struct TAccumulate<List, Op, I, true> {
		using type = I;
	};

	// =================================================

	template<typename List, template<typename...>class T>
	struct TExistInstance<List, T, false, true> : std::false_type {};

	template<typename List, template<typename...>class T, bool isEmpty>
	struct TExistInstance< List, T, true, isEmpty> : std::true_type {};

	template<typename List, template<typename...>class T>
	struct TExistInstance<List, T, false, false> : TExistInstance<TPopFront_t<List>, T, is_instance_of_v<TFront_t<List>, T>> {};

	// =================================================

	template<typename List, typename LastT, template<typename...>class T>
	struct TSearchInstance<List, LastT, T, false, true> { }; // no 'type'

	template<typename List, typename LastT, template<typename...>class T, bool isEmpty>
	struct TSearchInstance<List, LastT, T, true, isEmpty> {
		using type = LastT;
	};

	template<typename List, typename LastT, template<typename...>class T>
	struct TSearchInstance<List, LastT, T, false, false> : TSearchInstance<TPopFront_t<TTemplateType<List>::template type_template>, TFront_t<List>, T, is_instance_of_v<TFront_t<List>, T>> {};

	// =================================================
	//精妙，表示List中的每种类型是否唯一
	template<typename List, template<typename, typename> typename Equal>
	struct TIsUnique<List, Equal,true> : std::false_type {};
	template<template<typename...> typename TList, template<typename, typename> typename Equal>
	struct TIsUnique<TList<>, Equal,false> : std::true_type {};
	template<template<typename...> typename TList, typename Head, typename... Tail ,template<typename, typename> typename Equal>
	struct TIsUnique<TList<Head, Tail...>, Equal,false> : TIsUnique<TList<Tail...>, Equal, TContain_v<TList<Tail...>, Head, Equal>> {};
}
