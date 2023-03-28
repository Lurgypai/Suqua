#pragma once
#include <exception>

#include <string>

template<typename MissingClass>
class PoolNotFoundException : public std::exception {
public:
	virtual const char* what() const noexcept;
private:
	std::string wt;
};