#pragma once
#include <exception>
#include <string>

class FileNotFoundException : public std::exception {
public:
	FileNotFoundException(const std::string & file);
	virtual const char* what() const noexcept;
private:
	std::string filename;
};