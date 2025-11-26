#pragma once
#include <vector>
#include <array>
#include <memory>

#include <iostream>

template<typename T>
struct Resource {
	T val;
	bool isFree;

	Resource() : isFree{ true } {}
	template<typename U>
	Resource(U&& val_, bool isFree_) : val{ std::forward<U>(val_) }, isFree{ isFree_ } {}
	Resource(const Resource& other) = default;
	Resource(Resource&& other) = default;
	Resource& operator=(const Resource& other) = default;
	Resource& operator=(Resource&& other) = default;
};

class IPool {
public:
	virtual ~IPool() {};
	virtual bool free(std::size_t index) = 0;
};

template<typename T>
class Pool : public IPool {

public:
    static constexpr std::size_t CHUNK_SIZE = 256;
    using Chunk = std::unique_ptr<std::array<Resource<T>, CHUNK_SIZE>>;
    using ChunkedArray = std::vector<Chunk>;

	template<typename U>
	class PoolIterator {
	public:
		explicit PoolIterator(std::size_t pos_, std::size_t chunk_pos_, Resource<U> * start, ChunkedArray* pool_) : pos{ pos_ }, chunk_pos{chunk_pos_}, current{ start }, pool{pool_} {
            while(current && current->isFree) {
                if(pos < CHUNK_SIZE - 1){
                    ++current;
                    ++pos;
                } else {
                    // move to next chunk
                    pos = 0;
                    ++chunk_pos;
                    
                    // if we're out of chunks return. this is the end now.
                    if(chunk_pos == pool->size()) {
                        current = nullptr;
                        break;
                    }

                    current = &(*pool)[chunk_pos]->front();
                }
            }
		}
		PoolIterator(const PoolIterator& other) : pos{ other.pos }, current{ other.current } {}
		//preincrement
		PoolIterator& operator++() {
			do {
                if(pos < CHUNK_SIZE - 1){
                    ++current;
                    ++pos;
                } else {
                    // move to next chunk
                    pos = 0;
                    ++chunk_pos;
                    
                    // if we're out of chunks return. this is the end now.
                    if(chunk_pos == pool->size()) {
                        current = nullptr;
                        break;
                    }

                    current = &((*pool)[chunk_pos]->front());
                }
			} while (current && current->isFree);
			return *this;
		}
		//postincrement
		PoolIterator operator++(int) { PoolIterator retval = *this; ++(*this); return retval; }
		bool operator==(const PoolIterator & other) const { return current == other.current && pos == other.pos && chunk_pos == other.chunk_pos && pool == other.pool; }
		bool operator!=(const PoolIterator & other) const { return !(*this == other); }
		T& operator*() { return current->val; }
		T* operator->() { return &current->val; }
	private:
		std::size_t pos;
        std::size_t chunk_pos;
		Resource<T> * current;
        ChunkedArray* pool;
	};

	using iterator = PoolIterator<T>;

	Pool() : resources{}, size_{0} {}

	~Pool() {}

    template<typename U>
    inline void add(std::size_t pos, U&& u) {
        auto chunkId = pos / CHUNK_SIZE;
        auto chunkOffset = pos % CHUNK_SIZE;

        while(chunkId >= resources.size()) {
            resources.emplace_back(std::make_unique<std::array<Resource<T>, CHUNK_SIZE>>());
        }

        auto& rsrc = (*resources[chunkId])[chunkOffset];

        rsrc.isFree = false;
        rsrc.val = std::forward<U>(u);
        ++size_;
    }

	//Frees specified indice. Returns false if the indice is out of range.
	bool free(std::size_t index) override {
        if (index >= resources.size() * CHUNK_SIZE) return false;

        std::size_t chunkId = index / CHUNK_SIZE;
        std::size_t chunkOffset = index % CHUNK_SIZE;

        auto& target = (*resources[chunkId])[chunkOffset];
        
        if (!target.isFree) {
            --size_;
            target.isFree = true;
        }
        return true;
	}

	//Sets all indices to free.
	void clear() {
		for (auto& c : resources) {
            for(auto& r : c) {
                r.isFree = true;
            }
		}
        size_ = 0;
	}


	std::size_t size() const {
        return size_;
	}

	iterator begin() {
        // if empty don't try to set current
        if(resources.empty()) return end();

		return iterator{ 0, 0, resources.front()->data(), &resources};
	}

	iterator end() {
		return iterator{ 0, resources.size(), nullptr, &resources};
	}

	T& operator[](std::size_t i) {
        auto chunkId = i / CHUNK_SIZE;
        auto chunkOffset = i % CHUNK_SIZE;

		return (*resources[chunkId])[chunkOffset].val;
	}

	bool empty() {
		return size_ == 0;
	}

	//whether the specified indice is valid to pull data from.
	bool contains(std::size_t pos) {
        auto chunkId = pos / CHUNK_SIZE;
        auto chunkOffset = pos % CHUNK_SIZE;

		return pos >= 0 && chunkId < resources.size() && chunkOffset < CHUNK_SIZE && !(*resources[chunkId])[chunkOffset].isFree;
	}
private:
	ChunkedArray resources;
    std::size_t size_;
};

/*make pool a subclass (virtual dtor)
add type_info to pool
pool manager to generate new pools (unique_ptrs) and store them together in a vector
*/
