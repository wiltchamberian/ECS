#include "ECSMgr.h"
#include "ECSGlobal.h"


namespace Sun {

#if (ECS_STATIC_MODE ==1)
	std::vector<EntityInfo> ECSMgrBase::entityTable_;
	std::vector<std::size_t> ECSMgrBase::freeEntries_;
	std::unique_ptr<ECSMemoryPool> ECSMgrBase::memoryPool_(new ECSMemoryPool());
	tf::Taskflow ECSMgrBase::taskFlow_;
	tf::Executor ECSMgrBase::executor_;
	std::map<const Chunk*, SVector<TaskPackage>> ECSMgrBase::packages_;
#endif
	
#if 0
	SVector<ArchetypeMgr*> ECSMgr::filterArchetype(const ArchetypeFilter& filter) const {
		SVector<ArchetypeMgr*> res;
		for (auto& it : archeTypeMap_) {
			bool cond1 = true;
			for (auto& id : filter.all) {
				if (it.first.find(id) == it.first.end()) {
					cond1 = false;
					break;
				}
			}
			bool cond2 = true;
			for (auto& id : filter.any) {
				cond2 = false;
				if (it.first.find(id) != it.first.end()) {
					cond2 = true;
					break;
				}
			}
			bool cond3 = true;
			for (auto& id : filter.none) {
				if (it.first.find(id) != it.first.end()) {
					cond3 = false;
					break;
				}
			}
			if (cond1 && cond2 && cond3) {
				res.emplace_back(it.second.get());
			}
		}
		return res;
	}
#endif

	void ECSMgrBase::addPackage(const TaskPackage& package) {
		packages_[package.chunk].push_back(package);
	}

	void ECSMgrBase::runAllTasks() {
		for (auto& it : packages_) {
			if (it.second.empty()) continue;
			//作用于相同chunk的task,添加依赖关系
			tf::Task pre = taskFlow_.emplace(it.second[0]);
			for (size_t i = 1; i < it.second.size() - 1; ++i) {
				tf::Task t = taskFlow_.emplace(it.second[i]);
				pre.precede(t);
				pre = t;
			}
		}
		executor_.run(taskFlow_).wait();
		taskFlow_.clear();
	}

	template <typename C, std::enable_if_t<tf::is_condition_task_v<C>, void>*>
	tf::Task ECSMgrBase::emplaceTask(C&& task) {
		return taskFlow_.emplace(task);
	}
	
}
