#pragma once

#include "shader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include <string>
#include <exception>
#include <ft2build.h> // checking if build was good
#include FT_FREETYPE_H


class RenderTextException : std::exception {
private:
	std::string msg;
public:
	RenderTextException(const std::string& msg)
		:msg(msg) {}

	std::string what() {
		return msg;
	}
};

struct Character {
	unsigned int TextureID;
	glm::ivec2 Size; //w, h
	glm::ivec2 Bearing; //bearingX, bearingY (offset from basline to left/top of the glyph)
	unsigned int Advance;
};


class TextRenderer {
public:
	enum Mode {BOTTOM_LEFT, CENTER, TOP_LEFT, TOP_RIGHT, BOTTOM_RIGHT};
	void renderText(Shader& shader, std::string text, float x, float y, float scale, glm::vec3 color,
					Mode mode = BOTTOM_LEFT);
	int addCharacterToStorage(const char c);
	TextRenderer();
	TextRenderer(const std::string& font);
	~TextRenderer();

private:
	FT_Library ft;
	FT_Face face;
	unsigned int VAO, VBO;
	std::map<char, Character> Characters;
	void initializeStuff();
	const std::string fontPath;
};