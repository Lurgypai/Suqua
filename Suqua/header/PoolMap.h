#pragma once
#include "Pool.h"
#include <memory>
#include <vector>

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
	static void add(size_t pos, U&& u) {
		if (PoolWrapper<U>::pool == nullptr) {
			PoolWrapper<U>::pool = std::make_unique<Pool<U>>();
			pools.emplace_back(PoolWrapper<U>::pool.get());
		}
		PoolWrapper<U>::pool->add(pos, std::forward<U>(u));
	}

	template<typename T>
	static Pool<T>& get() {
		return *PoolWrapper<T>::pool;
	}

	template<typename T>
	static bool contains() {
		return PoolWrapper<T>::pool != nullptr;
	}

	static std::vector<pool_ptr>& getPools() {
		return pools;
	}
private:
	inline static std::vector<pool_ptr> pools{};
};
