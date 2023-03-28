#include "PoolNotFoundException.h"

template<typename MissingClass>
const char * PoolNotFoundException<MissingClass>::what() const noexcept {
	std::string wt = "Missing pool. Described by template parameter MissingClass.";
	return wt.c_str();
}
