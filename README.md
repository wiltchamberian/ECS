# ECS
This is an implement of the ECS archetecture in Game by C++17

⭐ Star us on GitHub — it helps!

# ECS

**S**un **E**ntity-**C**omponent-**S**ystem

# Compiler compatibility
> c++17

> Tested platforms:

> - Windows 7 : VS2019 16.10.2

# Helper Document
ECSMgr
管理所有的实体和系统，通常定义为唯一的全局变量，其模板参数为支持的原型

Archetype
原型，表示组件的组合，一个原型管理着Chunk的数组，Chunk中以SOA形式存储的这些组件。其模板参数为该原型所包含的组件

TEntity
实体，模板参数为其对应的原型，内部数据其实是表示一个句柄。

ArchetypeIter
原型迭代器，可以遍历相同原型的所有实体的，内部实现为一级索引；虽然通过entity也可以访问到组件，但是entity是2级索引，对于遍历，用迭代器效率更高

如果想给entity添加组件，由于本框架采用强类型实现，一旦entity添加组件，其原型就改变了，相当于其类型改变了。
因此统一通过createEntity接口，添加组件等效为删除再创建。详情见Example中的createEntity(std::move(...)

## Example
```c++
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
```

