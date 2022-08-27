# ECS
It may be the most brief implement of ECS in the world by c++20, 
no run-time overload, single head file, enjoy it!

⭐ Star us on GitHub — it helps!

# ECS

**S**un **E**ntity-**C**omponent-**S**ystem

# Compiler compatibility
> c++20

> Tested platforms:

> - Windows 7,8.1 : VS2019 16.10.2

# Helper Document
EntityMgr
管理所有的实体和系统，通常定义为唯一的全局变量，其模板参数为支持的原型

Archetype
原型，表示组件的组合，一个原型管理着Chunk的数组，Chunk中以SOA形式存储的这些组件。其模板参数为该原型所包含的组件

Entity
实体，内部数据其实是表示一个句柄。

ArchetypeIter
原型迭代器，可以遍历相同原型的所有实体的，内部实现为一级索引；虽然通过entity也可以访问到组件，但是entity是2级索引，对于遍历，用迭代器效率更高

# Update
1, 从先前的TEntity<...>改为了Entity,这样支持修改entity类型,使用更便捷
2，将entityTable移动到了EntityMgr中，这样移动entity时外部索引保持有效性
3，全部合并为单头文件实现，并做了大量代码优化，
   总共不到700行代码，史上最简洁的ecs实现，无运行期hash


## Example
```c++
void test(){
	using ArchetypeA = Archetype<int, float, AAA>;
	using ArchetypeB = Archetype<int, float, AAA, char>;
	using ArchetypeC = Archetype<float, AAA>;

	EntityMgr<ArchetypeA, ArchetypeB, ArchetypeC> mgr;
	static_assert(mgr.getArchetypeNum() == 3);
	static_assert(mgr.getComponentNum() == 4);

	Entity ea1 = mgr.createEntity< ArchetypeA>();
	mgr.get<AAA>(ea1).mass = 100;
	Entity ea2 = mgr.createEntity< ArchetypeA>();
	mgr.get<AAA>(ea2).mass = 100;
	Entity ea3 = mgr.createEntity< ArchetypeA>();
	mgr.get<AAA>(ea3).mass = 100;

	Entity eb1 = mgr.createEntity< ArchetypeB>();
	mgr.get<AAA>(eb1).mass = 50;
	Entity eb2 = mgr.createEntity< ArchetypeB>();
	mgr.get<AAA>(eb2).mass = 50;
	Entity eb3 = mgr.createEntity< ArchetypeB>();
	mgr.get<AAA>(eb3).mass = 50;

	Entity ec1 = mgr.createEntity< ArchetypeC>();
	mgr.get<AAA>(ec1).mass = 20;
	Entity ec2 = mgr.createEntity< ArchetypeC>();
	mgr.get<AAA>(ec2).mass = 20;
	Entity ec3 = mgr.createEntity< ArchetypeC>();
	mgr.get<AAA>(ec3).mass = 20;

	ArchetypeIter<ArchetypeB> iter = mgr.begin<ArchetypeB>();
	ArchetypeIter<ArchetypeB> iter2 = mgr.end<ArchetypeB>();
	for (iter; iter != iter2; ++iter) {
		iter->get<AAA>().mass -= 50;
	}
	assert(mgr.get<AAA>(eb1).mass == 0);
	assert(mgr.get<AAA>(eb2).mass == 0);
	assert(mgr.get<AAA>(eb3).mass == 0);

	std::function<void(AAA& aaa, float& f, char c)> func = [](AAA& aaa, float& f, char c) {
		aaa.mass += 50;
	};
	mgr.entity_for_each(func);
	mgr.runAllTasks();
	assert(mgr.get<AAA>(eb1).mass == 50);
	assert(mgr.get<AAA>(eb2).mass == 50);
	assert(mgr.get<AAA>(eb3).mass == 50);

	//移动entity(等价于修改Entity的类型)
	mgr.moveEntity<ArchetypeB>(ea2);
	assert(mgr.exist(ea2) == true);
	assert(mgr.get<AAA>(ea2).mass == 100);

	assert(mgr.getArchetypeEntityNum<ArchetypeA>() == 2);
	assert(mgr.getArchetypeEntityNum<ArchetypeB>() == 4);
	assert(mgr.getArchetypeEntityNum<ArchetypeC>() == 3);
	assert(mgr.getTotalEntityNum() == 9);

	std::cout << "All test passed!\n";
}

```

