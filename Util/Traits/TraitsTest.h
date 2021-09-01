#ifndef __TRAITS_H
#define __TRAITS_H

#include <functional>
#include <tuple>
#include "TypeList.hpp"

namespace Sun {
	class TestEntityAccessPtr {
	public:
		template<class _ComType>
		_ComType getComponent() {
			return _ComType(); //just make compiler pass
		};

	};

	template<typename List>
	struct go_test {
		void operator()(List input) {

		}
	};

	//template<typename ... _T>
	//using TypeList = std::tuple<_T...>;

	template<typename ... _Args>
	struct go_test<std::tuple<_Args...>> {
		void operator() (_Args... input) {

		}
	};

#if 0 //序列抽取函数
	//计算原型id

	using tuple_type = typename _Archetype::tuple_type;
	constexpr size_t siz = std::tuple_size_v<tuple_type>;
	auto seq = std::make_integer_sequence<size_t, siz>();

	template<typename _ComType>
	void Sun::ECSMgr::extractComTypeId(Sun::ArchetypeId& vec) {
		//vec.emplace(ComTypeId::buildComTypeId<_ComType>());
		vec.emplace(comTypeCounter_.unique_type_id<_ComType>());
	}

	template<typename _Archetype, typename _T, size_t... N>
	void Sun::ECSMgr::extractArchetypeId(Sun::ArchetypeId& id, std::integer_sequence<_T, N...> seq) {
		(extractComTypeId<typename _Archetype::template type<N>>(id), ...);
	}
#endif 

	template<typename _TypeInTuple>
	void extractInner(_TypeInTuple& ele,TestEntityAccessPtr ptr) {
		ele = ptr.getComponent<_TypeInTuple>();
	}

	template<typename Tuple, typename _T, size_t... N>
	void extract(Tuple& tp, TestEntityAccessPtr ptr, std::integer_sequence<_T, N...> seq) {
		(extractInner<std::tuple_element_t<N, Tuple>>(std::get<N>(tp),ptr), ...);
	}

	template<typename ... _Args>
	std::function<void(TestEntityAccessPtr)> test_regist(std::function<void(_Args...)>&& func) {
		using ArgList = TypeList<_Args...>;

		std::function<void(TestEntityAccessPtr)> wrapperfun = [&func](TestEntityAccessPtr ptr) {
			//从ptr中提取信息存入inpuptList
			std::tuple<_Args...> inputList;
			constexpr size_t siz = std::tuple_size_v<std::tuple<_Args...>>;
			auto seq = std::make_integer_sequence<size_t, siz>();
			extract(inputList, ptr, seq);

			func(std::get<_Args>(inputList)...);
		};

		return wrapperfun;
	}

	class Package {
		TestEntityAccessPtr ptr;
		std::function<void()> func_;
	};




}

#endif