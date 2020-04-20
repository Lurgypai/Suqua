#pragma once

#include <string>

struct Shader {
	unsigned int id;

	Shader();
	Shader(std::string vertPath, std::string fragPath);
	Shader(const Shader& other);

	void loadFromFiles(std::string vertPath, std::string fragPath);

	void use() const;

	void uniform1f(std::string val, float arg0) const;
	void uniform2f(std::string val, float arg0, float arg1) const;
	void uniform3f(std::string val, float arg0, float arg1, float arg2) const;

	void uniform1i(std::string val, int arg0) const;
};