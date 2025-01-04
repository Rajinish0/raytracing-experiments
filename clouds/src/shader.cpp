#include "shader.h"
#include <exception>

std::string Shader::loadSource(std::string source) {
	std::ifstream file{source};
	if (!file) {
		throw std::runtime_error("Could not load file " + source);
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

Shader::Shader(std::string vertexSource,
	std::string fragmentSource,
	std::string geometrySource) 
	:shaderProg(glCreateProgram()){

	std::string vSourceCode = loadSource(vertexSource);
	std::string fSourceCode = loadSource(fragmentSource);
	std::string gSourceCode = "";
	if (geometrySource != "")
		gSourceCode = loadSource(geometrySource);


	unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vSourceCode);
	checkCompileErrors(vertexShader, "VERTEX");
	unsigned int fragShader = compileShader(GL_FRAGMENT_SHADER, fSourceCode);
	checkCompileErrors(fragShader, "FRAGMENT");
	unsigned int geomShader;
	if (geometrySource != "") {
		geomShader = compileShader(GL_GEOMETRY_SHADER, gSourceCode);
		checkCompileErrors(geomShader, "GEOMETRY");
	}


	glAttachShader(shaderProg, vertexShader);
	if (geometrySource != "")
		glAttachShader(shaderProg, geomShader);
	glAttachShader(shaderProg, fragShader);
	glLinkProgram(shaderProg);
	checkCompileErrors(shaderProg, "PROGRAM");

	glDeleteShader(vertexShader);
	glDeleteShader(fragShader);
	if (geometrySource != "")
		glDeleteShader(geomShader);

}

void Shader::setBool(const std::string& name, bool value) const {
	glUniform1i(getLoc(name), (int)value);
}

void Shader::setInt(const std::string& name, int value) const {
	glUniform1i(getLoc(name), value);
}

void Shader::setFloat(const std::string& name, float value) const {
	glUniform1f(getLoc(name), value);
}

void Shader::setVec3(const std::string& name, glm::vec3 value) const {
	glUniform3f(getLoc(name), value.x, value.y, value.z);
}

void Shader::setVec2(const std::string& name, glm::vec2 value) const {
	glUniform2f(getLoc(name), value.x, value.y);
}

void Shader::setMatrix(const std::string& name, glm::mat4 value) const{
	// unsigned int loc = glGetUniformLocation(shaderProg, name.c_str());
	glUniformMatrix4fv(getLoc(name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setUniformBlockBinding(const std::string& name, int bindingPoint) const {
	unsigned int name_idx = glGetUniformBlockIndex(shaderProg, name.c_str());
	glUniformBlockBinding(shaderProg, name_idx, bindingPoint);
}

unsigned int Shader::getLoc(const std::string& name) const{
		return glGetUniformLocation(shaderProg, name.c_str());
}

unsigned int Shader::compileShader(GLenum type, std::string& sourceCode) {
	
	unsigned int shdr = glCreateShader(type);
	const char* src = sourceCode.c_str();

	glShaderSource(shdr, 1, &src, NULL);
	glCompileShader(shdr);

	return shdr;

}

void Shader::use() {
	glUseProgram(shaderProg);
}

