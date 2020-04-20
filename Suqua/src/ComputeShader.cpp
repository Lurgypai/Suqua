#include <fstream>
#include <sstream>
#include "glad/glad.h"
#include "ComputeShader.h"
#include <iostream>

ComputeShader::ComputeShader() : id{0} {}

ComputeShader::ComputeShader(std::string path) {
		std::ifstream shaderFile;
		shaderFile.open(path);

		std::stringstream shaderStream;
		shaderStream << shaderFile.rdbuf();
		std::string srcString = shaderStream.str();
		const char* shaderCode = srcString.c_str();

		unsigned int shader;
		shader = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(shader, 1, &shaderCode, NULL);
		glCompileShader(shader);

		
		int success;
		char infoLog[1024];
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "Compute Shader Failure:" << std::endl << infoLog << std::endl;
		}

		shaderFile.close();
		
		id = glCreateProgram();
		glAttachShader(id, shader);
		glLinkProgram(id);

		
		glGetProgramiv(id, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(id, 1024, NULL, infoLog);
			std::cout << "Linking of Shaders Failed:" << std::endl << infoLog << std::endl;
		}
		
		glDeleteShader(shader);
		
}

ComputeShader::ComputeShader(const ComputeShader & other) :
	id{other.id}
{}

void ComputeShader::uniform2f(const std::string & var, float x, float y) {
	glUniform2f(glGetUniformLocation(id, var.c_str()), x, y);
}

void ComputeShader::uniform1f(const std::string & var, float x) {
	glUniform1f(glGetUniformLocation(id, var.c_str()), x);
}

void ComputeShader::use() const {
	glUseProgram(id);
}