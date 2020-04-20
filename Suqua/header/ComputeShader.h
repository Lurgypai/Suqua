#pragma once

#include <string>

struct ComputeShader {
	unsigned int id;

	ComputeShader();
	ComputeShader(std::string path);
	ComputeShader(const ComputeShader& other);

	void loadFromFiles(std::string path);

	void uniform2f(const std::string & var, float x, float y);
	void uniform1f(const std::string & var, float x);

	void use() const;
};