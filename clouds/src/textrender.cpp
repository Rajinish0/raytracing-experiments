#include "textrender.h"


TextRenderer::TextRenderer() 
	:fontPath("fonts/arial.ttf") {
	initializeStuff();
}

TextRenderer::TextRenderer(const std::string& font)
	:fontPath(font) {
	initializeStuff();
}

int TextRenderer::addCharacterToStorage(const char c) {
	if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
		std::cerr << "Failed to load glyph for charac: \"" << c
			<< '"';
		return -1;
	}

	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width,
		face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	Character charac{
		texture,
		glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
		glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
		face->glyph->advance.x };
	Characters[c] = charac;

	return 0;
}

void TextRenderer::initializeStuff() {
	if (FT_Init_FreeType(&ft)) {
		throw RenderTextException("Could not init freetype lib");
	}

	if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) {
		throw RenderTextException("Error, failed to load font");
	}

	FT_Set_Pixel_Sizes(face, 48, 48);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);


	for (unsigned char c = 0; c < 128; c++)
		addCharacterToStorage(c);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// quad has 6 verts plus 4 coords per vert
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

void TextRenderer::renderText(Shader& shader, std::string text, 
							  float x, float y, float scale, 
							  glm::vec3 color, Mode mode) {
	shader.use();
	shader.setVec3("textColor", color);

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);
	Character ch;

	float totalWidth = 0.0f;
	float totalHeight = 0.0f;
	for (const char c : text) {
		ch = Characters[c];
		totalWidth += (ch.Advance >> 6) * scale;
		totalHeight = std::max(totalHeight, (float)ch.Size.y);
	}


	float xOffset, yOffset;
	switch (mode) {
		case BOTTOM_LEFT:
			xOffset = 0.0f;
			yOffset = 0.0f;
			break;
		case CENTER:
			xOffset = -totalWidth / 2.0f;
			yOffset = -totalHeight / 2.0f;
			break;
		case TOP_LEFT:
			xOffset = 0.0f;
			yOffset = -totalHeight;
			break;
		case TOP_RIGHT:
			xOffset = -totalWidth;
			yOffset = -totalHeight;
			break;
		case BOTTOM_RIGHT:
			xOffset = -totalWidth;
			yOffset = 0.0f;
			break;
	}

	for (const char c : text) {
		ch = Characters[c];

		/*
		* x, y define the bottom left of the text
		* the explanation for (ch.Size.y - ch.Bearing.y) is pretty easy to understand
		*
		* what i dont understand is why are texture coordinates flipped?
		* if xpos, ypos is the bottom left
		* then xpos, ypos + h should be the top left
		* why then are the texture coordinates there are 0, 0?
		*
		* Are the texture coords for glyphs defined s.t. 0, 0 is at top left?
		*/

		float xpos = (x + xOffset) + ch.Bearing.x * scale;
		float ypos = (y + yOffset) - (ch.Size.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;

		float verts[]{
			xpos, ypos + h, 0.0f, 0.0f,
			xpos, ypos,		0.0f, 1.0f,
			xpos + w, ypos, 1.0f, 1.0f,

			xpos, ypos + h, 0.0f, 0.0f,
			xpos + w, ypos, 1.0f, 1.0f,
			xpos + w, ypos + h, 1.0f, 0.0f
		};

		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		x += (ch.Advance >> 6) * scale;// 1/64 * ch.Advance is the value in pixels for how much to go to the next x
		// but 1/64 * ch.Advance is just >> 6 (bitshift right)
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

TextRenderer::~TextRenderer() {
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}