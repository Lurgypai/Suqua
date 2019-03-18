#include "stdafx.h"
#include "FileNotFoundException.h"

FileNotFoundException::FileNotFoundException(const std::string & file) {
	filename = file;
}

const char * FileNotFoundException::what() const noexcept {
	std::string wt{ "File \"" + filename + "\" not found." };
	return wt.c_str();
}
