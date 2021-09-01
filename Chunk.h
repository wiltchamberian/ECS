/*****************************************************************************
* @brief : Chunk
* 由ArchetypeMgr所管理，代表一块内存区域
* @author : acedtang
* @date : 2021/8/19
* @version : ver 1.0
* @inparam :
* @outparam :
*****************************************************************************/

#ifndef __SUN_ECS_CHUNK_H
#define __SUN_ECS_CHUNK_H

#include <cstdint>

namespace Sun {

#define SUN_CHUNK_SIZE 16384

	struct Chunk {
		//该Chunk上真实的object个数
		size_t numEntity_ = 0;
		//存储object数据
		uint8_t data_[SUN_CHUNK_SIZE];
	};
}

#endif