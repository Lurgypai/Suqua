#pragma once
#include <exception>

#include <string>
#include "TypeData.h"

class PoolNotFoundException : public std::exception {
public:
	PoolNotFoundException(type_id id_);
	virtual const char* what() const noexcept;
private:
	std::string wt;
	type_id id;
};