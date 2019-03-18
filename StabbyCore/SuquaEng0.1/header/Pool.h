#pragma once
#include <vector>

template<typename T>
struct Resource {
	T val;
	bool isFree;

	Resource() : isFree{ true } {}
	template<typename U>
	Resource(U&& val_, bool isFree_) : val{ std::forward<U>(val_) }, isFree{ isFree_ } {}
};

class IPool {
public:
	virtual ~IPool() {};
};

template<typename T, int Size = 0>
class Pool : public IPool {

public:

	Pool() {
		resources.resize(Size);
	}

	Pool(std::size_t size) {
		resources.resize(size);
	}

	~Pool() {}

	//Add value in free indice, or resize.
	template <typename U>
	inline void add(U&& r) {

		for (Resource<T>& res : resources) {
			if (res.isFree) {
				res = Resource<T>(std::forward<U>(r), false);
				return;
			}
		}
		resources.emplace_back(std::forward<U>(r), false);
	}

	inline void add() {
		for (Resource<T>& res : resources) {
			if (res.isFree) {
				res = Resource<T>(T{}, false);
				return;
			}
		}
		resources.emplace_back(T{}, false);
	}

	//Frees specified indice. Returns false if the indice is out of range.
	bool free(int index) {
		if (index < resources.size()) {
			resources[index].isFree = true;
			return true;
		}
		return false;
	}

	//Sets all indices to free.
	void clear() {
		for (auto& r : resources) {
			r.isFree = true;
		}
	}

	void resize(std::size_t s) {
		resources.resize(s);
	}

	std::size_t size() const {
		return resources.size();
	}

	const std::vector<Resource<T>>& get() {
		return resources;
	}

	auto begin() {
		return resources.begin();
	}

	auto end() {
		return resources.end();
	}

	Resource<T>& operator[](std::size_t i) {
		return resources[i];
	}

	Resource<T>& front() {
		return resources.front();
	}

	Resource<T>& back() {
		return resources.back();
	}

	bool empty() {
		return resources.empty();
	}

private:
	std::vector<Resource<T>> resources;
};

/*make pool a subclass (virtual dtor)
add type_info to pool
pool manager to generate new pools (unique_ptrs) and store them together in a vector
*/