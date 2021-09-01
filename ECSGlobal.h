/*****************************************************************************
* @brief :ECSGlobal
* ECS一些公共全局变量
* @author : acedtang
* @date : 2021/8/19
* @version : ver 1.0
* @inparam :
* @outparam :
*****************************************************************************/

#ifndef __SUN_ECS_GLOBAL_H
#define __SUN_ECS_GLOBAL_H

#include <cstdint>
#include <vector>
#include <functional>
#include <atomic>

namespace Sun {

#define ECS_STATIC_MODE 0
#if (ECS_STATIC_MODE==1)
#define ECS_HEAD static 
#define ECS_TAIL
#else
#define ECS_HEAD  
#define ECS_TAIL const
#endif

	struct Chunk;

	template<class _T>
	using SVector = std::vector<_T>;

	struct EntityAddress {
		std::size_t chunkIdx{ static_cast<std::size_t>(-1) };
		std::size_t idxInChunk{ static_cast<std::size_t>(-1) };
	};
	struct EntityInfo : public EntityAddress{
		std::uint32_t version{ 0 }; // version
	};

	struct MemberFunctions {

		//缺省placement构造函数
		std::function<void(void*)> default_constructor;

		//拷贝构造函数
		std::function<void(void*, const void*)> copy_constructor;

		//移动构造函数
		std::function<void(void*, void*)> move_constructor;

		//移动赋值
		std::function<void(void*, void*)> move_assignment;

		//析构函数
		std::function<void(void*)> destructor;
	};
	
	struct ComTypeInfo : public MemberFunctions {
		size_t siz = 0 ;
		size_t align_siz = 0;
	};

	
#define SUN_ECS_ALIGNMENT  4

	
}


#endif