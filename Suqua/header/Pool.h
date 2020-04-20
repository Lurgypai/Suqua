#pragma once
#include <vector>

template<typename T>
struct Resource {
	T val;
	bool isFree;

	Resource() : isFree{ true } {}
	template<typename U>
	Resource(U&& val_, bool isFree_) : val{ std::forward<U>(val_) }, isFree{ isFree_ } {}
	Resource(const Resource & other) : val{ other.val }, isFree{ other.isFree } {}
	Resource & operator=(const Resource & other) {
		val = other.val;
		isFree = other.isFree;
		return *this;
	}
};

class IPool {
public:
	virtual ~IPool() {};
	virtual bool free(size_t index) = 0;
};

template<typename T>
class Pool : public IPool {

public:

	template<typename U>
	class PoolIterator {
	public:
		PoolIterator() : size{ 0 }, pos{ 0 }, current{ nullptr } {}
		explicit PoolIterator(size_t pos_, size_t size_, Resource<U> * start) : size{ size_ }, pos{ pos_ }, current{ start } {
			while (pos < size && current->isFree) {
				++current;
				++pos;
			}
		}
		PoolIterator(const PoolIterator& other) : size{ other.size }, pos{ other.pos }, current{ other.current } {}
		//preincrement
		PoolIterator& operator++() {
			do {
				++current;
				++pos;
			} while (pos < size && current->isFree);
			return *this;
		}
		//postincrement
		PoolIterator operator++(int) { PoolIterator retval = *this; ++(*this); return retval; }
		bool operator==(const PoolIterator & other) const { return current == other.current && size == other.size && pos == other.pos; }
		bool operator!=(const PoolIterator & other) const { return !(*this == other); }
		T& operator*() { return current->val; }
		T* operator->() { return &current->val; }
	private:
		size_t size;
		size_t pos;
		Resource<T> * current;
	};

	using iterator = PoolIterator<T>;

	Pool() : resources{}, freeIndices_{ 0 } {}

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

	template <typename U>
	inline void add(size_t pos, U&& r) {
		if (resources.size() > pos) {
			if (resources[pos].isFree)
				--freeIndices_;
			resources[pos] = Resource<T>(std::forward<U>(r), false);
		}
		else {
			//add in betweens
			size_t start = resources.size();
			resize(pos);
			free(start, pos);
			//place us on end
			resources.emplace_back(std::forward<U>(r), false);
		}
	}

	inline void add(size_t pos) {
		if (resources.size() > pos) {
			resources[pos] = Resource<T>(T{}, false);
		}
		else {
			size_t start = resources.size();
			resize(pos);
			free(start, pos);
			resources.emplace_back(T{}, false);
		}
	}

	//Frees specified indice. Returns false if the indice is out of range.
	bool free(size_t index) override {
		if (index < resources.size()) {
			if (!resources[index].isFree)
				++freeIndices_;
			resources[index].isFree = true;
			return true;
		}
		return false;
	}

	bool free(int start, int end) {
		if (start > 0 && end <= resources.size()) {
			for (int i = start; i != end; ++i) {
				if (!resources[i].isFree)
					++freeIndices_;
				resources[i].isFree = true;
			}
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
		if (s > resources.size())
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

	std::size_t capacity() const {
		return resources.size();
	}

	std::size_t size() const {
		return resources.size() - freeIndices_;
	}

	std::size_t freeIndices() const {
		return freeIndices_;
	}

	bool full() const {
		return freeIndices_ == 0;
	}

	const std::vector<Resource<T>>& get() const {
		return resources;
	}

	iterator begin() {
		return iterator{ 0, resources.size(), resources.data() };
	}

	iterator end() {
		return iterator{ resources.size(), resources.size(), resources.data() + resources.size() };
	}

	auto beginResource() {
		return resources.begin();
	}

	auto endResource() {
		return resources.end();
	}

	T& operator[](std::size_t i) {
		return resources[i].val;
	}

	T& front() {
		for (auto& resource : resources) {
			if (!resource.isFree)
				return resource.val;
		}
	}

	T& back() {
		for (auto reverseIter = resources.rbegin(); reverseIter != resources.rend(); ++reverseIter) {
			if (!reverseIter->isFree)
				return reverseIter->val;
		}
	}

	bool empty() {
		return resources.empty();
	}

	auto data() {
		return resources.data();
	}

	//whether the specified indice is valid to pull data from.
	bool contains(size_t pos) {
		return pos >= 0 && pos < resources.size() && !resources[pos].isFree;
	}


	Resource<T> & getResource(size_t pos) {
		return resources[pos];
	}
private:
	std::vector<Resource<T>> resources;
	std::size_t freeIndices_;
};

/*make pool a subclass (virtual dtor)
add type_info to pool
pool manager to generate new pools (unique_ptrs) and store them together in a vector
*/