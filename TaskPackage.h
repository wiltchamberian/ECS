#ifndef __SUN_ECS_PACKAGE_H
#define __SUN_ECS_PACKAGE_H

#include "Chunk.h"

namespace Sun {

	struct TaskPackage {
		const Chunk* chunk = nullptr;
		std::function<void()> func;
		void operator()() {
			func();
		}
	};
}

#endif