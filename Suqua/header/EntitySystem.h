#pragma once
#include <vector>
#include <unordered_map>
#include <iostream>
#include "PoolMap.h"

using EntityId = uint64_t;

//you may need to alter the inner workings of makeComps, and how components are stored
//but make sure that the implementation can change without changinging the interface

class EntitySystem {
public:

	template<typename T>
	static T* GetComp(EntityId id);

	template<typename T>
	static void MakeComps(unsigned int size, EntityId* first);

	static void GenEntities(unsigned int num, EntityId* idStore);
	static void FreeEntities(unsigned int num, const EntityId* idStore);
	static void FreeDeadEntities();

	template<typename T>
	static Pool<T>& GetPool();

	template<typename T>
	static bool Contains();

	template<typename T>
	static void FreeComps(unsigned int size, EntityId* first);

	void Init(size_t entityMax);
private:
	static size_t entityMax;
	static EntityId entityIncrem;
};

template<typename T>
inline T* EntitySystem::GetComp(EntityId id) {
	if (id == 0)
		throw std::exception{};
	auto& pool = PoolMap::get<T>();
	if (pool.contains(id - 1))
		return &pool[id - 1];
	else
		return nullptr;
}

//TODO 
template<typename T>
inline void EntitySystem::MakeComps(unsigned int size, EntityId* first) {
	for (int i = 0; i != size; i++) {
		if (first[i] > 0)
			PoolMap::add<T>(first[i] - 1, first[i]);
	}
}

template<typename T>
inline Pool<T>& EntitySystem::GetPool() {
	return PoolMap::get<T>();
}

template<typename T>
inline bool EntitySystem::Contains() {
	return PoolMap::contains<T>();
}

template<typename T>
inline void EntitySystem::FreeComps(unsigned int size, EntityId* first) {
	for (int i = 0; i != size; ++i) {
		for (auto iter = PoolMap::get<T>().beginResource(); iter != PoolMap::get<T>().endResource(); ++iter) {
			auto& comp = *iter;
			if (comp.val.getId() == first[i])
				comp.isFree = true;
		}
	}
}