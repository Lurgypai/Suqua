#pragma once
#include "Pool.h"
#include <memory>
#include <vector>
#include <unordered_map>
#include "TypeData.h"

using pool_ptr = std::unique_ptr<IPool>;

class PoolMap {
public:
	template<typename U>
	void add(U&& u) {
		if (pools.find(TypeTag<U>::tag) == pools.end()) {

			pools.insert(std::pair<type_id, pool_ptr>{ TypeTag<U>::tag, std::make_unique<Pool<U>>()});
		}

		Pool<U>* pool = static_cast<Pool<U>*>(pools[TypeTag<U>::tag].get());
		pool->add(std::forward<U>(u));
	}

	template<typename U>
	void add() {
		if (pools.find(TypeTag<U>::tag) == pools.end()) {

			pools.insert(std::pair<type_id, pool_ptr>{ TypeTag<U>::tag, std::make_unique<Pool<U>>()});
		}

		Pool<U>* pool = static_cast<Pool<U>*>(pools[TypeTag<U>::tag].get());
		pool->add();
	}

	template<typename T>
	Pool<T>* get() {
		if (pools.find(TypeTag<T>::tag) != pools.end()) {
			return static_cast<Pool<T>*>(pools[TypeTag<T>::tag].get());
		}
		else {
			return nullptr;
		}
	}
private:
	std::unordered_map<type_id, pool_ptr> pools;
};