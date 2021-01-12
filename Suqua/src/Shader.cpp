#include <iostream>
#include "Shader.h"

#include <fstream>
#include <sstream>

#include <glad/glad.h>
#include <SDL.h>

Shader::Shader() : id{0} {}

Shader::Shader(std::string vertPath, std::string fragPath) {
	std::ifstream vShaderFile;
	vShaderFile.open(vertPath);

	std::stringstream vShaderStream;
	vShaderStream << vShaderFile.rdbuf();
	std::string vSrcString = vShaderStream.str();
	const char* vShaderCode = vSrcString.c_str();

	unsigned int vertex;
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);

	int success;
	char infoLog[1024];
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex, 1024, NULL, infoLog);
		std::cout << "Vertex Shader Failure:" << std::endl << infoLog << std::endl;
	}

	vShaderFile.close();

	std::ifstream fShaderFile;
	fShaderFile.open(fragPath);

	std::stringstream fShaderStream;
	fShaderStream << fShaderFile.rdbuf();
	std::string fSrcString = fShaderStream.str();
	const char* fShaderCode = fSrcString.c_str();

	unsigned int fragment;
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);

	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 1024, NULL, infoLog);
		std::cout << "Fragment Shader Failure:" << std::endl << infoLog << std::endl;
	}

	fShaderFile.close();

	id = glCreateProgram();
	glAttachShader(id, vertex);
	glAttachShader(id, fragment);
	glLinkProgram(id);


	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(id, 1024, NULL, infoLog);
		std::cout << "Linking of Shaders Failed:" << std::endl << infoLog << std::endl;
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

Shader::Shader(const Shader & other) :
	id{ other.id }
{
}

void Shader::loadFromFiles(std::string vertPath, std::string fragPath) {
	if (id != 0)
		glDeleteProgram(id);

	std::ifstream vShaderFile;
	vShaderFile.open(vertPath);

	std::stringstream vShaderStream;
	vShaderStream << vShaderFile.rdbuf();
	std::string vSrcString = vShaderStream.str();
	const char* vShaderCode = vSrcString.c_str();

	unsigned int vertex;
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);

	int success;
	char infoLog[1024];
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex, 1024, NULL, infoLog);
		std::cout << "Vertex Shader Failure:" << std::endl << infoLog << std::endl;
	}

	vShaderFile.close();

	std::ifstream fShaderFile;
	fShaderFile.open(fragPath);

	std::stringstream fShaderStream;
	fShaderStream << fShaderFile.rdbuf();
	std::string fSrcString = fShaderStream.str();
	const char* fShaderCode = fSrcString.c_str();

	unsigned int fragment;
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);

	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragment, 1024, NULL, infoLog);
		std::cout << "Fragment Shader Failure:" << std::endl << infoLog << std::endl;
	}

	fShaderFile.close();

	id = glCreateProgram();
	glAttachShader(id, vertex);
	glAttachShader(id, fragment);
	glLinkProgram(id);


	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(id, 1024, NULL, infoLog);
		std::cout << "Linking of Shaders Failed:" << std::endl << infoLog << std::endl;
	}

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::use() const{
	glUseProgram(id);
}

void Shader::uniform1f(std::string var, float arg0) const {
	glUniform1f(glGetUniformLocation(id, var.c_str()), arg0);
}

void Shader::uniform2f(std::string var, float arg0, float arg1) const {
	glUniform2f(glGetUniformLocation(id, var.c_str()), arg0, arg1);
}

void Shader::uniform3f(std::string var, float arg0, float arg1, float arg2) const {
	glUniform3f(glGetUniformLocation(id, var.c_str()), arg0, arg1, arg2);
}

void Shader::uniform4f(std::string var, float arg0, float arg1, float arg2, float arg3) const {
	glUniform4f(glGetUniformLocation(id, var.c_str()), arg0, arg1, arg2, arg3);
}

void Shader::uniform1i(std::string val, int arg0) const {
	glUniform1i(glGetUniformLocation(id, val.c_str()), arg0);
}

void Shader::uniform2i(std::string val, int arg0, int arg1) const {
	glUniform2i(glGetUniformLocation(id, val.c_str()), arg0, arg1);
}

void Shader::uniform1ui(std::string val, unsigned int arg0) const {
	glUniform1ui(glGetUniformLocation(id, val.c_str()), arg0);
}