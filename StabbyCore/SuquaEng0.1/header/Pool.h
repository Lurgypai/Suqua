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
		freeIndices_ = size;
	}

	~Pool() {}

	//Add value in free indice, or resize.
	template <typename U>
	inline void add(U&& r) {

		for (Resource<T>& res : resources) {
			if (res.isFree) {
				res = Resource<T>(std::forward<U>(r), false);
				--freeIndices_;
				return;
			}
		}
		resources.emplace_back(std::forward<U>(r), false);
	}

	inline void add() {
		for (Resource<T>& res : resources) {
			if (res.isFree) {
				res = Resource<T>(T{}, false);
				--freeIndices_;
				return;
			}
		}
		resources.emplace_back(T{}, false);
	}

	//Frees specified indice. Returns false if the indice is out of range.
	bool free(int index) {
		if (index < resources.size()) {
			resources[index].isFree = true;
			++freeIndices_;
			return true;
		}
		return false;
	}

	//Sets all indices to free.
	void clear() {
		for (auto& r : resources) {
			r.isFree = true;
		}
		freeIndices_ = resources.size();
	}

	void resize(std::size_t s) {
		if(s > resources.size())
			freeIndices_ += s - resources.size();
		else {
			//count the amount of free elemnts that are being removed
			std::size_t freeElements = 0;
			for (auto i = s; i != resources.size(); ++i) {
				freeElements += resources[i].isFree;
			}
			//remove the removed free indices
			freeIndices_ -= freeElements;
		}
		resources.resize(s);
	}

	std::size_t size() const {
		return resources.size();
	}

	std::size_t freeIndices() const {
		return freeIndices_;
	}

	bool full() const {
		return freeIndices_ == 0;
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

	auto data() {
		return resources.data();
	}

private:
	std::vector<Resource<T>> resources;
	std::size_t freeIndices_;
};

/*make pool a subclass (virtual dtor)
add type_info to pool
pool manager to generate new pools (unique_ptrs) and store them together in a vector
*/