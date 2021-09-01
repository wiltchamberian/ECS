//this is a example 

#include <iostream>
#include "ECSMgr.h"

using namespace Sun;

struct AAA {
	int mass;
	AAA() {
		mass = 0;
	}
	AAA(const AAA& other) {
		this->mass = other.mass;
	}
};

int main()
{
	using ArchetypeA = Archetype<int, float, AAA>;
	using ArchetypeB = Archetype<int, float, AAA, char>;
	using ArchetypeC = Archetype<float, AAA>;

	ECSMgr<ArchetypeA, ArchetypeB, ArchetypeC> mgr;
	static_assert(mgr.getArchetypeNum() == 3);

	TEntity<ArchetypeA> ea1 = mgr.createEntity< ArchetypeA>();
	mgr.get<AAA>(ea1).mass = 100;
	TEntity<ArchetypeA> ea2 = mgr.createEntity< ArchetypeA>();
	mgr.get<AAA>(ea2).mass = 100;
	TEntity<ArchetypeA> ea3 = mgr.createEntity< ArchetypeA>();
	mgr.get<AAA>(ea3).mass = 100;

	TEntity<ArchetypeB> eb1 = mgr.createEntity< ArchetypeB>();
	mgr.get<AAA>(eb1).mass = 50;
	TEntity<ArchetypeB> eb2 = mgr.createEntity< ArchetypeB>();
	mgr.get<AAA>(eb2).mass = 50;
	TEntity<ArchetypeB> eb3 = mgr.createEntity< ArchetypeB>();
	mgr.get<AAA>(eb3).mass = 50;

	TEntity<ArchetypeC> ec1 = mgr.createEntity< ArchetypeC>();
	mgr.get<AAA>(ec1).mass = 20;
	TEntity<ArchetypeC> ec2 = mgr.createEntity< ArchetypeC>();
	mgr.get<AAA>(ec2).mass = 20;
	TEntity<ArchetypeC> ec3 = mgr.createEntity< ArchetypeC>();
	mgr.get<AAA>(ec3).mass = 20;

	ArchetypeIter<ArchetypeB> iter = mgr.begin<ArchetypeB>();
	ArchetypeIter<ArchetypeB> iter2 = mgr.end<ArchetypeB>();
	for (iter; iter != iter2; ++iter) {
		iter->get<AAA>().mass -= 50;
	}
	int k = mgr.get<AAA>(eb2).mass;
	assert(mgr.get<AAA>(eb2).mass == 0);

	std::function<void(AAA& aaa, float& f, char c)> func = [](AAA& aaa, float& f, char c) {
		aaa.mass += 50;
	};
	mgr.entity_for_each(func);
	mgr.runAllTasks();
	assert(mgr.get<AAA>(eb2).mass == 50);

	//移动entity(由于强类型，移动等价于删除再创建,但是组件会拷贝过去)
	auto eb4 = mgr.createEntity<ArchetypeB>(std::move(ea2));
	assert(mgr.exist(ea2) == false);
	assert(mgr.get<AAA>(eb4).mass == 100);

    std::cout << "All test passed!\n";
}