#include "compute_shader.h"
#include <exception>

std::string ComputeShader::loadSource(std::string source) {
	std::ifstream file{source};
	if (!file) {
		throw std::runtime_error("Could not load file " + source);
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

ComputeShader::ComputeShader(std::string sourceCode) 
	:shaderProg(glCreateProgram())
{
    unsigned int compute;

    compute = glCreateShader(GL_COMPUTE_SHADER);

	std::string SourceCode = loadSource(sourceCode);

	unsigned int computeShader = compileShader(GL_COMPUTE_SHADER, SourceCode);
	checkCompileErrors(computeShader, "COMPUTE");

	glAttachShader(shaderProg, computeShader);
	glLinkProgram(shaderProg);
	checkCompileErrors(shaderProg, "PROGRAM");

	glDeleteShader(computeShader);
}

void ComputeShader::setBool(const std::string& name, bool value) const {
	glUniform1i(getLoc(name), (int)value);
}

void ComputeShader::setInt(const std::string& name, int value) const {
	glUniform1i(getLoc(name), value);
}

void ComputeShader::setFloat(const std::string& name, float value) const {
	glUniform1f(getLoc(name), value);
}

void ComputeShader::setVec3(const std::string& name, glm::vec3 value) const {
	glUniform3f(getLoc(name), value.x, value.y, value.z);
}

void ComputeShader::setVec2(const std::string& name, glm::vec2 value) const {
	glUniform2f(getLoc(name), value.x, value.y);
}

void ComputeShader::setMatrix(const std::string& name, glm::mat4 value) const{
	// unsigned int loc = glGetUniformLocation(shaderProg, name.c_str());
	glUniformMatrix4fv(getLoc(name), 1, GL_FALSE, glm::value_ptr(value));
}

void ComputeShader::setUniformBlockBinding(const std::string& name, int bindingPoint) const {
	unsigned int name_idx = glGetUniformBlockIndex(shaderProg, name.c_str());
	glUniformBlockBinding(shaderProg, name_idx, bindingPoint);
}

unsigned int ComputeShader::getLoc(const std::string& name) const{
		return glGetUniformLocation(shaderProg, name.c_str());
}

unsigned int ComputeShader::compileShader(GLenum type, std::string& sourceCode) {
	
	unsigned int shdr = glCreateShader(type);
	const char* src = sourceCode.c_str();

	glShaderSource(shdr, 1, &src, NULL);
	glCompileShader(shdr);

	return shdr;
}

void ComputeShader::use() {
	glUseProgram(shaderProg);
}

