#include "FileNotFoundException.h"

FileNotFoundException::FileNotFoundException(const std::string & file) :
	filename{file},
	wt{ "File \"" + file + "\" not found." }
{}

const char * FileNotFoundException::what() const noexcept {
	return wt.c_str();
}
