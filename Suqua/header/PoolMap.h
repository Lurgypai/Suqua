#pragma once
#include "Pool.h"
#include <memory>
#include <set>
#include <unordered_map>
#include <list>

#include "TypeData.h"
#include "PoolNotFoundException.h"

using pool_ptr = IPool*;

template<typename P>
using unique_pool_ptr = std::unique_ptr<Pool<P>>;

class PoolMap {
private:
	template<typename P>
	class PoolWrapper {
	public:
		inline static unique_pool_ptr<P> pool = nullptr;
	};

public:
	template<typename U>
	void add(U&& u) {
		if (PoolWrapper<U>::pool == nullptr) {
			PoolWrapper<U>::pool = std::make_unique<Pool<U>>();
			pools.emplace_back(PoolWrapper<U>::pool.get());
		}

		PoolWrapper<U>::pool->add(std::forward<U>(u));
	}

	template<typename U>
	void add(size_t pos, U&& u) {
		if (PoolWrapper<U>::pool == nullptr) {
			PoolWrapper<U>::pool = std::make_unique<Pool<U>>();
			pools.emplace_back(PoolWrapper<U>::pool.get());
		}
		PoolWrapper<U>::pool->add(pos, std::forward<U>(u));
	}

	template<typename U>
	void add() {
		if (PoolWrapper<U>::pool == nullptr) {
			PoolWrapper<U>::pool = std::make_unique<Pool<U>>();
			pools.emplace_back(PoolWrapper<U>::pool.get());
		}
		PoolWrapper<U>::pool->add();
	}

	template<typename T>
	Pool<T> & get() {
		return *PoolWrapper<T>::pool;
	}

	template<typename T>
	bool contains() {
		return PoolWrapper<T>::pool != nullptr;
	}

	auto begin() {
		return pools.begin();
	}

	auto end() {
		return pools.end();
	}

private:

	std::list<pool_ptr> pools;
};
