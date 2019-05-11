#pragma once

#include <string>

struct ComputeShader {
	unsigned int id;

	ComputeShader();
	ComputeShader(std::string path);
	ComputeShader(const ComputeShader& other);

	void loadFromFiles(std::string path);

	void use() const;
};