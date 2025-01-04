// /*
// * EXPLORING TEXT RENDERING (will seperate into a seperate library later)
// */


// #include <iostream>
// #include "shader.h"
// #include "camera.h"
// #include <glad/glad.h>
// #include <GLFW/glfw3.h>
// //#ifndef STB_IMAGE_IMPLEMENTATION
// //#define STB_IMAGE_IMPLEMENTATION
// //#endif
// #include "stb_image.h"
// #include "funcs.h"
// #include "window.h"
// #include "model.h"
// #include <map>
// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtc/type_ptr.hpp>
// #include "textrender.h"
// //#include <ft2build.h> // checking if build was good
// //#include FT_FREETYPE_H


// void framebuffer_size_callback(GLFWwindow* window, int width, int height);
// void processInput(GLFWwindow* window);
// void mouse_callback(GLFWwindow* window, double xPos, double yPos);



// float vertices[] = {
// 	// positions          // texture Coords
// 	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
// 	 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
// 	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
// 	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
// 	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
// 	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

// 	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
// 	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
// 	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
// 	 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
// 	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
// 	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

// 	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
// 	-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
// 	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
// 	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
// 	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
// 	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

// 	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
// 	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
// 	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
// 	 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
// 	 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
// 	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

// 	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
// 	 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
// 	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
// 	 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
// 	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
// 	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

// 	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
// 	 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
// 	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
// 	 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
// 	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
// 	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
// };

// float planeVertices[] = {
// 	// positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
// 	 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
// 	-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
// 	-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

// 	 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
// 	-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
// 	 5.0f, -0.5f, -5.0f,  2.0f, 2.0f
// };


// /*
// * just realized this is kind of stupid, because
// * i could have used planeVertices and passed 6 instead of
// * 36 in glDrawArrays
// */
// float grassVertices[]{
// 	-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
// 	 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
// 	 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
// 	 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
// 	-0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
// 	-0.5f, -0.5f, 0.0f, 0.0f, 0.0f
// };

// float quadvertices[]{
// 	-1.0f, -1.0f, 0.0f, 0.0f,
// 	 1.0f, -1.0f, 1.0f, 0.0f,
// 	 1.0f,  1.0f, 1.0f, 1.0f,
// 	 1.0f,  1.0f, 1.0f, 1.0f,
// 	-1.0f,  1.0f, 0.0f, 1.0f,
// 	-1.0f, -1.0f, 0.0f, 0.0f
// };

// float simpleQuad[]{
// 	-0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
// 	 0.5f,-0.5f, 0.0f, 1.0f, 0.0f,
// 	-0.5f,-0.5f, 0.0f, 0.0f, 1.0f,


// 	-0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
// 	 0.5f,-0.5f, 0.0f, 1.0f, 0.0f,
// 	 0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
// };

// float squareQuad[]{
// 	0.0f,	0.0f, 0.0f,
// 	500.0f, 0.0f, 0.0f,
// 	500.0f, 500.0f, 0.0f,

// 	0.0f, 0.0f, 0.0f,
// 	500.0f, 500.0f, 0.0f,
// 	0.0f, 500.0f, 0.0f
// };

// unsigned int indicies[] = {
// 	0, 1, 2,
// 	0, 2, 3
// };

// glm::vec3 cubePositions[] = {
// 	glm::vec3(0.0f,  0.0f,  0.0f),
// 	glm::vec3(2.0f,  5.0f, -15.0f),
// 	glm::vec3(-1.5f, -2.2f, -2.5f),
// 	glm::vec3(-3.8f, -2.0f, -12.3f),
// 	glm::vec3(2.4f, -0.4f, -3.5f),
// 	glm::vec3(-1.7f,  3.0f, -7.5f),
// 	glm::vec3(1.3f, -2.0f, -2.5f),
// 	glm::vec3(1.5f,  2.0f, -2.5f),
// 	glm::vec3(1.5f,  0.2f, -1.5f),
// 	glm::vec3(-1.3f,  1.0f, -1.5f)
// };



// unsigned int cubeVBO, cubeVAO, planeVAO, planeVBO,
// grassVAO, grassVBO, fbo, rbo, quadVAO, uboBlock,
// quadVBO, squareVAO, squareVBO, instanceVBO,
// asteroidVBO, sqVAO, sqVBO;
// unsigned int vertexShader;
// unsigned int fragShader;
// unsigned int texture, texture2,
// grassTexture, fbtexture;
// float dt = 0.007f;
// float lastTime = NULL;

// Camera cam;
// std::vector<glm::vec3> vegetation;


// unsigned int textVAO, textVBO;
// //
// //struct Character {
// //	unsigned int TextureID;
// //	glm::ivec2 Size; //w, h
// //	glm::ivec2 Bearing; //bearingX, bearingY (offset from basline to left/top of the glyph)
// //	unsigned int Advance;
// //};
// //
// //std::map<char, Character> Characters;
// //
// //
// //void renderText(Shader& shader, std::string text, float x, float y, float scale, glm::vec3 color) {
// //	shader.use();
// //	shader.setVec3("textColor", color);
// //
// //	glActiveTexture(GL_TEXTURE0);
// //	glBindVertexArray(textVAO);
// //	for (const char c : text) {
// //		Character ch = Characters[c];
// //
// //		/*
// //		* x, y define the bottom left of the text
// //		* the explanation for (ch.Size.y - ch.Bearing.y) is pretty easy to understand
// //		* 
// //		* what i dont understand is why are texture coordinates flipped?
// //		* if xpos, ypos is the bottom left
// //		* then xpos, ypos + h should be the top left
// //		* why then are the texture coordinates there are 0, 0?
// //		* 
// //		* Are the texture coords for glyphs defined s.t. 0, 0 is at top left?
// //		*/
// //
// //		float xpos = x + ch.Bearing.x * scale;
// //		float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
// //
// //		float w = ch.Size.x * scale;
// //		float h = ch.Size.y * scale;
// //
// //		float verts[]{
// //			xpos, ypos + h, 0.0f, 0.0f,
// //			xpos, ypos,		0.0f, 1.0f,
// //			xpos + w, ypos, 1.0f, 1.0f,
// //
// //			xpos, ypos + h, 0.0f, 0.0f,
// //			xpos + w, ypos, 1.0f, 1.0f,
// //			xpos + w, ypos + h, 1.0f, 0.0f
// //		};
// //
// //		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
// //		glBindBuffer(GL_ARRAY_BUFFER, textVBO);
// //		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
// //		glDrawArrays(GL_TRIANGLES, 0, 6);
// //		x += (ch.Advance >> 6) * scale;// 1/64 * ch.Advance is the value in pixels for how much to go to the next x
// //									   // but 1/64 * ch.Advance is just >> 6 (bitshift right)
// //	}
// //
// //	glBindVertexArray(0);
// //	glBindTexture(GL_TEXTURE_2D, 0);
// //}


// int main() {
// 	stbi_set_flip_vertically_on_load(true);


// 	Window window(800, 600);


// 	glfwSetFramebufferSizeCallback(window.window, framebuffer_size_callback);
// 	glfwSetCursorPosCallback(window.window, mouse_callback);

// 	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
// 	{
// 		std::cout << "Failed to initliaze GLAD" << std::endl;
// 		return -1;
// 	}

// 	glm::vec4 v(1.0f, 1.0f, 1.0f, 0.0f);

// 	Shader myshdr{ "shaders/cvert.glsl", "shaders/cfrag.glsl" };
// 	Shader myshdr2{ "shaders/cvert.glsl", "shaders/borderfrag.glsl" };
// 	// Shader myshdr3{ "cvert.glsl", "grassfrag.glsl" };
// 	// Shader squareShader{ "squareV.glsl", "squareF.glsl", "squareGeom.glsl" };
// 	// Shader modelShader{ "shader2.glsl", "cfrag.glsl", "modelGeom.glsl" };
// 	// Shader normalVizShader{ "normalViz-vshader.glsl", "normalViz-frag.glsl",
// 	// 						"normalViz-geom.glsl" };
// 	// Shader instancingShader{ "instArray-vshader.glsl",
// 	// 						 "instancing-fshader.glsl" };
// 	// Shader pShader{ "model-vshader.glsl",
// 	// 				   "model-fshader.glsl" };

// 	// Shader aShader{ "asteroid-vshader.glsl",
// 	// 			   "model-fshader.glsl" };
// 	// Shader orthShader{"orthotest.vert",
// 	// 				  "orthotest.frag"};
// 	Shader textShader{ "shaders/textVshader.glsl",
// 					  "shaders/textFshader.glsl" };

// 	Model planet{ "models/planet/planet.obj" };
// 	Model asteriod{ "models/rock/rock.obj" };
// 	TextRenderer textRenderer;


// 	glEnable(GL_DEPTH_TEST);
// 	glEnable(GL_BLEND);
// 	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

// 	glGenBuffers(1, &cubeVBO);
// 	glGenBuffers(1, &planeVBO);
// 	glGenBuffers(1, &grassVBO);
// 	glGenBuffers(1, &quadVBO);
// 	glGenBuffers(1, &squareVBO);
// 	glGenBuffers(1, &uboBlock);
// 	glGenBuffers(1, &instanceVBO);
// 	glGenBuffers(1, &asteroidVBO);
// 	glGenBuffers(1, &sqVBO);

// 	texture = funcs::TextureFromFile("container2.png", "D:");
// 	texture2 = funcs::TextureFromFile("earth2.png", "D:");
// 	grassTexture = funcs::TextureFromFile("grass.png", "D:");

// 	glGenVertexArrays(1, &cubeVAO);
// 	glGenVertexArrays(1, &planeVAO);
// 	glGenVertexArrays(1, &grassVAO);
// 	glGenVertexArrays(1, &quadVAO);
// 	glGenVertexArrays(1, &squareVAO);
// 	glGenVertexArrays(1, &sqVAO);


// 	glBindVertexArray(cubeVAO);
// 	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);


// 	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

// 	//vpos
// 	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
// 	glEnableVertexAttribArray(0);
// 	//tcoord
// 	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
// 	glEnableVertexAttribArray(1);

// 	glBindVertexArray(planeVAO);
// 	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);

// 	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);

// 	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
// 	glEnableVertexAttribArray(0);

// 	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
// 	glEnableVertexAttribArray(1);

// 	glBindBuffer(GL_ARRAY_BUFFER, 0);
// 	glBindVertexArray(0);

// 	glBindVertexArray(grassVAO);
// 	glBindBuffer(GL_ARRAY_BUFFER, grassVBO);

// 	glBufferData(GL_ARRAY_BUFFER, sizeof(grassVertices), grassVertices, GL_STATIC_DRAW);

// 	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
// 	glEnableVertexAttribArray(0);

// 	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
// 	glEnableVertexAttribArray(1);

// 	glBindBuffer(GL_ARRAY_BUFFER, 0);
// 	glBindVertexArray(0);

// 	glBindVertexArray(quadVAO);
// 	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);

// 	glBufferData(GL_ARRAY_BUFFER, sizeof(quadvertices), quadvertices, GL_STATIC_DRAW);
// 	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
// 	glEnableVertexAttribArray(0);

// 	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
// 	glEnableVertexAttribArray(1);

// 	glBindVertexArray(squareVAO);
// 	glBindBuffer(GL_ARRAY_BUFFER, squareVBO);

// 	glBufferData(GL_ARRAY_BUFFER, sizeof(simpleQuad), simpleQuad, GL_STATIC_DRAW);
// 	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
// 	glEnableVertexAttribArray(0);

// 	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
// 	glEnableVertexAttribArray(1);

// 	glBindVertexArray(sqVAO);
// 	glBindBuffer(GL_ARRAY_BUFFER, sqVBO);
// 	glBufferData(GL_ARRAY_BUFFER, sizeof(squareQuad), squareQuad, GL_STATIC_DRAW);
// 	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
// 	glEnableVertexAttribArray(0);


// 	glBindBuffer(GL_ARRAY_BUFFER, 0);
// 	glBindVertexArray(0);


// 	glViewport(0, 0, 800, 600);

// 	glm::mat4 trans(1.0f);
// 	glm::mat4 model(1.0f);
// 	glm::mat4 lmodel(1.0f);
// 	glm::mat4 view(1.0f);
// 	glm::mat4 proj(1.0f);
// 	glm::vec3 lpos(1.2f, 0.0f, 1.0f);
// 	model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
// 	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
// 	proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 200.0f);



// 	glBindTexture(GL_TEXTURE_2D, texture2);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


// 	myshdr.use();
// 	myshdr.setInt("texture_diff", 0);


// 	//while (!glfwWindowShouldClose(window))
// 	glBindFramebuffer(GL_FRAMEBUFFER, 0);

// 	glm::vec2 translations[100];
// 	float offset = 0.1f; // NDC coords
// 	int index = 0;
// 	for (int y = -10; y < 10; y += 2) {
// 		for (int x = -10; x < 10; x += 2) {
// 			glm::vec2 trans(
// 				float(x) / 10.0f + offset,
// 				float(y) / 10.0f + offset
// 			);
// 			translations[index++] = trans;
// 		}
// 	}

// 	//instancingShader.use();
// 	//for (int i = 0; i < 100; ++i)
// 	//	instancingShader.setVec2("offsets["+std::to_string(i)+"]", translations[i]);

// 	glBindVertexArray(squareVAO);

// 	glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
// 	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 100, &translations[0], GL_STATIC_DRAW);

// 	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
// 	glEnableVertexAttribArray(2);
// 	glVertexAttribDivisor(2, 1);

// 	unsigned int amt = 100000;
// 	glm::mat4* modelMats = new glm::mat4[amt];
// 	srand(glfwGetTime());
// 	float rad = 50.0;
// 	offset = 25.0;

// 	for (unsigned int i = 0; i < amt; ++i) {
// 		glm::mat4 model(1.0f);
// 		float angle = (float)i / (float)amt * 360.0f;
// 		float disp = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
// 		float x = sin(angle) * rad + disp;
// 		disp = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
// 		float y = disp * 0.4f;
// 		disp = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
// 		float z = cos(angle) * rad + disp;

// 		model = glm::translate(model, glm::vec3(x, y, z));

// 		float scale = (rand() % 20) / 100.0f + 0.05;
// 		model = glm::scale(model, glm::vec3(scale));

// 		float rotA = (rand() % 360);
// 		model = glm::rotate(model, rotA, glm::vec3(0.4f, 0.6f, 0.8f));//im gonna make the axis random too later

// 		modelMats[i] = model;
// 	}

// 	glBindBuffer(GL_ARRAY_BUFFER, asteroidVBO);
// 	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * amt, &modelMats[0], GL_STATIC_DRAW);

// 	//FT_Library ft;
// 	//if (FT_Init_FreeType(&ft)) {
// 	//	std::cerr << "Could not init freetype lib" << std::endl;
// 	//	return -1;
// 	//}

// 	//FT_Face face;
// 	//if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face)) {
// 	//	std::cout << "Error, failed to load font" << std::endl;
// 	//	return -1;
// 	//}

// 	//FT_Set_Pixel_Sizes(face, 0, 48);


// 	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
// 	//for (unsigned char c = 0; c < 128; c++) {
// 	//	if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
// 	//		std::cerr << "Failed to load glyph for charac: \"" << c
// 	//				  << '"';
// 	//	}
// 	//	unsigned int texture;
// 	//	glGenTextures(1, &texture);
// 	//	glBindTexture(GL_TEXTURE_2D, texture);
// 	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width,
// 	//		face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
// 	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
// 	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
// 	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
// 	//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

// 	//	Character charac{
// 	//		texture,
// 	//		glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
// 	//		glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
// 	//		face->glyph->advance.x };
// 	//	Characters[c] = charac;
// 	//}

// 	//FT_Done_Face(face);
// 	//FT_Done_FreeType(ft);

// 	//glGenVertexArrays(1, &textVAO);
// 	//glGenBuffers(1, &textVBO);
// 	//glBindVertexArray(textVAO);
// 	//glBindBuffer(GL_ARRAY_BUFFER, textVBO);
// 	//// quad has 6 verts plus 4 coords per vert
// 	//glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
// 	//glEnableVertexAttribArray(0);
// 	//glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
// 	//glBindBuffer(GL_ARRAY_BUFFER, 0);
// 	//glBindVertexArray(0);

// 	//for (unsigned int i = 0; i < asteriod.meshes.size(); ++i) {
// 	//	glBindVertexArray(asteriod.meshes[i].VAO);
// 	//	std::size_t vec4Size = sizeof(glm::vec4);

// 	//	glEnableVertexAttribArray(3);
// 	//	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);

// 	//	glEnableVertexAttribArray(4);
// 	//	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));

// 	//	glEnableVertexAttribArray(5);
// 	//	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));

// 	//	glEnableVertexAttribArray(6);
// 	//	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

// 	//	glVertexAttribDivisor(3, 1);
// 	//	glVertexAttribDivisor(4, 1);
// 	//	glVertexAttribDivisor(5, 1);
// 	//	glVertexAttribDivisor(6, 1);

// 	//	glBindVertexArray(0);


// 	//}


// 	proj = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
// 	while (!window.shouldClose())
// 	{
// 		processInput(window.window);
// 		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
// 		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
// 		view = cam.getView();

// 		//orthShader.use();
// 		//orthShader.setMatrix("proj", proj);
// 		//
// 		//glBindVertexArray(sqVAO);
// 		//glDrawArrays(GL_TRIANGLES, 0, 6);

// 		textShader.use();
// 		textShader.setMatrix("proj", proj);
// 		textRenderer.renderText(textShader, "SACRE BLEUU", 400.0f, 300.0f, 1.0f,
// 			glm::vec3(1.0f, 0.5f, 0.0f), TextRenderer::CENTER);

// 		/*renderText(textShader, "Learning", 0.0f, 0.0f, 1.0f,
// 			glm::vec3(1.0f, 0.0f, 0.0f));*/
		


// 		//pShader.use();
// 		//pShader.setMatrix("proj", proj);
// 		//pShader.setMatrix("view", view);
// 		//pShader.setMatrix("model", glm::mat4(1.0f));
// 		//planet.draw(pShader);


// 		//aShader.use();
// 		//aShader.setMatrix("proj", proj);
// 		//aShader.setMatrix("view", view);
// 		//for (unsigned int i = 0; i < asteriod.meshes.size(); ++i) {
// 		//	glBindVertexArray(asteriod.meshes[i].VAO);
// 		//	glDrawElementsInstanced(
// 		//		GL_TRIANGLES, asteriod.meshes[i].indicies.size(), GL_UNSIGNED_INT, 0, amt
// 		//	);
// 		//}



// 		window.update();

// 	}

// 	glfwTerminate();
// 	return 0;
// }



// void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
// 	glViewport(0, 0, width, height);
// }

// void processInput(GLFWwindow* window)
// {

// 	if (!lastTime) {
// 		lastTime = glfwGetTime();
// 		return;
// 	}

// 	float cTime = glfwGetTime();
// 	dt = (cTime - lastTime) * 10;
// 	lastTime = cTime;

// 	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
// 		glfwSetWindowShouldClose(window, true);

// 	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
// 		cam.move(Camera::UP, dt);

// 	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
// 		cam.move(Camera::DOWN, dt);

// 	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
// 		cam.move(Camera::RIGHT, dt);

// 	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
// 		cam.move(Camera::LEFT, dt);

// 	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
// 		cam.yaw -= cam.sensitivity;
// 		cam.updateDirection();
// 	}

// 	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
// 		cam.yaw += cam.sensitivity;
// 		cam.updateDirection();
// 	}

// 	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
// 		//cam.pitch += cam.sensitivity;
// 		cam.incPitch(cam.sensitivity);
// 		cam.updateDirection();
// 	}

// 	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
// 		cam.incPitch(-cam.sensitivity);
// 		//cam.pitch -= cam.sensitivity;
// 		cam.updateDirection();
// 	}
// }

// void mouse_callback(GLFWwindow* window, double xPos, double yPos) {
// 	cam.handleMouse(xPos, yPos);
// }