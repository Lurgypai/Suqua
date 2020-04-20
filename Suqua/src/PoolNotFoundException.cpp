#include "PoolNotFoundException.h"

PoolNotFoundException::PoolNotFoundException(type_id id_) :
	id{id_},
	wt{ "Pool with ID " + std::to_string(id_) + " not found" }
{}

const char * PoolNotFoundException::what() const noexcept {
	return wt.c_str();
}
