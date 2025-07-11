/*
* FOR EXPLORING INSTANCING
*/


#include <iostream>
#include "shader.h"
#include "camera.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
//#ifndef STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_IMPLEMENTATION
//#endif
#include "stb_image.h"
#include "funcs.h"
#include "window.h"
#include "model.h"
#include "perlin.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "framebuffer.h"
#include "compute_shader.h"
#include "textrender.h"
#include <ft2build.h> // checking if build was good


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);




float quadVertices[] = {
    -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 
    -1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 
     1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 

    -1.0f,  1.0f, 0.0f, 1.0f, 0.0f, 
     1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 
     1.0f,  1.0f, 1.0f, 1.0f, 1.0f 
};

// float planeVerts[] = {
// 	-1.0f, -1.0f, 0.0f, 0.0f,
// 	-1.0f,  1.0f, 0.0f, 1.0f,
// 	 1.0f, -1.0f, 1.0f, 0.0f,

// 	-1.0f,  1.0f, 0.0f, 1.0f,
// 	 1.0f, -1.0f, 1.0f, 0.0f,
// 	 1.0f,  1.0f, 1.0f, 1.0f
// };

float planeVerts[]{
	-1.0f, -1.0f, 0.0f, 0.0f,
	 1.0f, -1.0f, 1.0f, 0.0f,
	 1.0f,  1.0f, 1.0f, 1.0f,
	 1.0f,  1.0f, 1.0f, 1.0f,
	-1.0f,  1.0f, 0.0f, 1.0f,
	-1.0f, -1.0f, 0.0f, 0.0f
};


// Vertex data for a unit cube centered at the origin
float cubeVertices[] = {
    // Positions          // Texture Coordinates (UVW)

    // Front face
    -0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f, // Bottom-left
     0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 1.0f, // Bottom-right
     0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f, // Top-right
    -0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 1.0f, // Top-left

    // Back face
    -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 0.0f, // Bottom-left
     0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f, // Bottom-right
     0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 0.0f, // Top-right
    -0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f, // Top-left

    // Left face
    -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 0.0f, // Bottom-left
    -0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f, // Bottom-right
    -0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 1.0f, // Top-right
    -0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f, // Top-left

    // Right face
     0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f, // Bottom-left
     0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 1.0f, // Bottom-right
     0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f, // Top-right
     0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 0.0f, // Top-left

    // Top face
    -0.5f,  0.5f, -0.5f,   0.0f, 1.0f, 0.0f, // Bottom-left
     0.5f,  0.5f, -0.5f,   1.0f, 1.0f, 0.0f, // Bottom-right
     0.5f,  0.5f,  0.5f,   1.0f, 1.0f, 1.0f, // Top-right
    -0.5f,  0.5f,  0.5f,   0.0f, 1.0f, 1.0f, // Top-left

    // Bottom face
    -0.5f, -0.5f, -0.5f,   0.0f, 0.0f, 0.0f, // Bottom-left
     0.5f, -0.5f, -0.5f,   1.0f, 0.0f, 0.0f, // Bottom-right
     0.5f, -0.5f,  0.5f,   1.0f, 0.0f, 1.0f, // Top-right
    -0.5f, -0.5f,  0.5f,   0.0f, 0.0f, 1.0f, // Top-left
};

// Index data for drawing the cube using element array
unsigned int cubeIndices[] = {
    0, 1, 2, 2, 3, 0,       // Front face
    4, 5, 6, 6, 7, 4,       // Back face
    8, 9, 10, 10, 11, 8,    // Left face
    12, 13, 14, 14, 15, 12, // Right face
    16, 17, 18, 18, 19, 16, // Top face
    20, 21, 22, 22, 23, 20  // Bottom face
};


float densityThreshold = .420003f;
float scale 		   = .595005f;
float weatherScale     = .0001f;
float higherScale 	   =  15.0f;
float SIGMA 		   = 0.9f;
float HG			   = 0.3f;

float noise[800 * 600];
Perlin2d perlin;


unsigned int quadVBO, quadVAO, 
			 planeVBO, planeVAO,
			 cubeVBO, cubeVAO, cuboEBO;
unsigned int texture2, skboxTexture;
float dt = 0.007f;
float lastTime = NULL;

//SPHERE TPYES
const int OPAQUE_T 	= 0x01; 
const int REFLECT_T = 0x02;
const int REFRACT_T = 0x04; 

//SPHERE TEXTURE TYPES
const int FROM_TEXTURE = 0x01;
const int FROM_COLOR   = 0x02;

std::vector<std::string> faces{
	"textures/faces/right.jpg",
	"textures/faces/left.jpg",
	"textures/faces/top.jpg",
	"textures/faces/bottom.jpg",
	"textures/faces/front.jpg",
	"textures/faces/back.jpg"
};



template <typename T>
T randNum(T min, T max){
	T dist = (max - min);
	float random_number = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
	return (min + dist*random_number);
}


Camera cam;

glm::vec3 lightPos = glm::vec3(-1.0f, 7.0f, 2.0f);


void updateNoise(){
	for (int i =0; i < 800; i++){
		for (int j =0; j < 600; j++){
			noise[j + i * 600] = perlin.perlin( ((float)j)/600.0f, ((float)i)/800.0f );
		}
	}
}


int main() {
	// stbi_set_flip_vertically_on_load(true);


	Window window(800, 600);


	glfwSetFramebufferSizeCallback(window.window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window.window, mouse_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initliaze GLAD" << std::endl;
		return -1;
	}

	 Shader rayShdr{
		"shaders/rayt-shader.glsl",
		"shaders/rayf.glsl"
	 };

	 Shader planeShdr {
		"shaders/planeV.glsl",
		"shaders/planeF.glsl"
	 };

	 Shader meshShdr {
		"shaders/meshV.glsl",
		"shaders/meshF.glsl"
	 };

	 Shader quadShdr {
		"shaders/quadV.glsl",
		"shaders/quadF.glsl"
	 };

	 Shader cubeShdr {
		"shaders/cubeV.glsl",
		"shaders/cubeF.glsl"
	 };

	 Shader textShdr {
		"shaders/textVshader.glsl",
		"shaders/textFshader.glsl"
	 };

	 ComputeShader computeShdr {
		"shaders/compute.glsl"
	 };

	 Shader screenShdr {
		"shaders/q_v.glsl",
		"shaders/q_f.glsl"
	 };

	glEnable(GL_DEPTH_TEST);

	FrameBuffer fbo;
	TextRenderer textRenderer;
	

	glGenBuffers(1, &quadVBO);
	glGenBuffers(1, &planeVBO);
	glGenVertexArrays(1, &planeVAO);

	// texture = funcs::TextureFromFile("container2.png", "D:");
	texture2 = funcs::TextureFromFile("earthmap.jpg", "textures/");
	skboxTexture = funcs::loadCubeMap(faces);

	glGenVertexArrays(1, &quadVAO);

	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	// glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*) 0);
	// glEnableVertexAttribArray(0);

	// glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(2 * sizeof(float)));
	// glEnableVertexAttribArray(1);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glBindVertexArray(planeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVerts), planeVerts, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(1);
	

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &cubeVBO);
	glGenBuffers(1, &cuboEBO);


	glBindVertexArray(cubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cuboEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);

	unsigned int compT;
	glGenTextures(1, &compT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, compT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 800, 600, 0, GL_RGBA, 
				GL_FLOAT, NULL);

	glBindImageTexture(0, compT, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);


	glViewport(0, 0, 800, 600);
	glm::mat4 trans(1.0f);
	glm::mat4 model(1.0f);
	glm::mat4 lmodel(1.0f);
	glm::mat4 view(1.0f);
	glm::mat4 proj(1.0f);
	glm::mat4 invProj(1.0f);
	glm::mat4 invView(1.0f);
	glm::vec3 lpos(1.2f, 0.0f, 1.0f);
	model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
	const float near = 0.1f;
	const float far  = 500.0f;
	proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, near, far);
	glm::mat4 planeModel = glm::rotate(
		glm::mat4(1.0f), 
		glm::radians(90.0f),
		glm::vec3(1.0f, 0.0f, 0.0f)
	);

	float sphereRad = 0.2f;
	glm::mat4 sphereModel = glm::mat4(1.0f);
	sphereModel = glm::translate(sphereModel, glm::vec3(-2 * sphereRad, sphereRad, 0.0f));
	sphereModel = glm::scale(sphereModel, glm::vec3(sphereRad));

	glm::mat4 sphereModel2 = glm::mat4(1.0f);
	sphereModel2 = glm::translate(sphereModel2, glm::vec3(2 * sphereRad, sphereRad, 0.0f));
	sphereModel2 = glm::scale(sphereModel2, glm::vec3(sphereRad));



	invProj = glm::inverse(proj);
	invView = glm::inverse(view);
	cam.position = glm::vec3(0.0f);
	Mesh sphere = funcs::genSphere();

	computeShdr.use();
	computeShdr.setVec3("camPos", cam.position);
	computeShdr.setVec3("bounding_rect.pos", glm::vec3(0.0f, 50.5f, 0.0f));
	computeShdr.setVec3("bounding_rect.dims", glm::vec3(400.0f, 200.0f, 400.0f));
	computeShdr.setFloat("near", near);
	computeShdr.setFloat("far", far);
	computeShdr.setMatrix("invProjMat", glm::inverse(proj));

	glm::vec3 lightPos(3.0f, 2.0f, 0.0f);
	fbo.setClearColor(glm::vec4(0.1f, 0.1f, 0.1f, 0.1f));
	GLuint ntId = funcs::genWorleyNoise(50, 50, 50);
	// GLuint weatherTextureId = funcs::loadWeatherData("weather_data.raw");
	GLuint weatherTextureId = funcs::loadWeatherData("weather_data_2.raw");
	GLuint detailTextureId = funcs::loadDetailTexture("low_res.raw");
	GLuint highTexture = funcs::loadGeneric3dTexture("f_data_HIGH.raw");
	std::cout << "LOADING WEATHER TEXTURE" << std::endl;
	GLuint weatherDataTexure = funcs::loadGeneric2dTexture("weather_data_f.raw");
	std::cout << "DONE loading textures" << std::endl;

	glm::mat4 cubeModel = glm::translate(
		glm::mat4(1.0f),
		glm::vec3(3.0f, 0.0f, 0.0f)
	);

	while (!window.shouldClose())
	{

		
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		processInput(window.window);
		glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		view = cam.getView();

		// std::cout << "sc: "<< scale << std::endl;
		// std::cout << "dt: "<< densityThreshold << std::endl;
		// std::cout << "wc: "<< weatherScale << std::endl;
		// std::cout << "hc: "<< higherScale << std::endl;

		lightPos = glm::rotate(
			glm::mat4(1.0f), 
			glm::radians(0.1f), 
			glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightPos, 0.0f);

		fbo.Bind();

		planeShdr.use();
		planeShdr.setVec3("lightPos", lightPos);
		planeShdr.setMatrix("proj", proj);
		planeShdr.setMatrix("view", view);
		planeShdr.setMatrix("model", planeModel);
		planeShdr.setInt("myTexture", 0);

		glActiveTexture(0);
		// glBindTexture(GL_TEXTURE_2D, weatherTextureId);
		glBindTexture(GL_TEXTURE_2D, weatherDataTexure);

		glBindVertexArray(planeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);


		meshShdr.use();
		meshShdr.setBool("isLight", false);
		//SPHERE1
		meshShdr.setVec3("lightPos", lightPos);
		meshShdr.setMatrix("proj", proj);
		meshShdr.setMatrix("view", view);
		meshShdr.setMatrix("model", sphereModel);
		sphere.draw(meshShdr);

		//SPHERE 2
		meshShdr.setMatrix("model", sphereModel2);
		sphere.draw(meshShdr);

		//LIGHT
		meshShdr.setBool("isLight", true);
		glm::mat4 sphereModel3 = glm::mat4(1.0f);
		sphereModel3 = glm::translate(sphereModel3, lightPos);
		sphereModel3 = glm::scale(sphereModel3, glm::vec3(sphereRad));

		meshShdr.setMatrix("model", sphereModel3);

		sphere.draw(meshShdr);

		glBindVertexArray(cubeVAO);
		cubeShdr.use();
		cubeShdr.setMatrix("proj", proj);
		cubeShdr.setMatrix("view", view);
		cubeShdr.setMatrix("model", cubeModel);
		cubeShdr.setInt("texture_diffuse", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, detailTextureId);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		textShdr.use();
		textShdr.setMatrix(
			"proj", proj
		);
		textRenderer.renderText(textShdr, "HELLO", 400.0f, 300.0f, 1.0f, glm::vec3(1.0f, 0.0f, 0.0f), TextRenderer::CENTER);

		fbo.unBind();

		computeShdr.use();
		computeShdr.setMatrix(
			"invViewMat",
			glm::inverse(view)
		);
		computeShdr.setVec3("camPos", cam.position);
		computeShdr.setVec3("offSet", glm::vec3(
			0.0f, 
			0.0f,
			(float)glfwGetTime()
		));

		
		glDisable(GL_BLEND);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, compT);
        computeShdr.setInt("texture_diffuse1", 1);
        computeShdr.setInt("depthTexture", 2);
		computeShdr.setInt("texture_clouds", 3);
		computeShdr.setInt("weather_data", 4);
		computeShdr.setInt("detailTexture", 5);
		computeShdr.setFloat("densityThreshold", densityThreshold);
		computeShdr.setFloat("scale", scale);
		computeShdr.setFloat("weatherScale", weatherScale);
		computeShdr.setFloat("higherScale", higherScale);
		computeShdr.setFloat("SIGMA", SIGMA);
		computeShdr.setFloat("HG", HG);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, fbo.textureId);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, fbo.depthTextureId);
		glActiveTexture(GL_TEXTURE3);
		// glBindTexture(GL_TEXTURE_3D, ntId);
		glBindTexture(GL_TEXTURE_3D, highTexture);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, weatherTextureId);
		// glBindTexture(GL_TEXTURE_2D, weatherDataTexure);
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_3D, detailTextureId);
		glDispatchCompute((unsigned int)std::ceil(800.0/16.0), 
						  (unsigned int)std::ceil(600.0/16.0),
						  1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		// fbo.draw(quadShdr);

		screenShdr.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, compT);
		glBindVertexArray(planeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// plane.draw(quadShdr, 0);

		window.update();

	}

	glfwTerminate();
	return 0;
}



void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{

	if (!lastTime) {
		lastTime = glfwGetTime();
		return;
	}

	float cTime = glfwGetTime();
	dt = (cTime - lastTime) * 5;
	lastTime = cTime;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cam.move(Camera::UP, dt*10.0f);

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cam.move(Camera::DOWN, dt*10.0f);

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cam.move(Camera::RIGHT, dt*10.0f);

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cam.move(Camera::LEFT, dt*10.0f);

	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		densityThreshold += 0.001f;
	
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		densityThreshold -= 0.001f;

	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		scale += 0.001f;
	
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		scale -= 0.001f;


	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		// higherScale += .01f;
		HG += 0.001;
	
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		// higherScale -= .01f;
		HG -= 0.001;

	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		weatherScale += 0.0001f;

	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
		weatherScale -= 0.0001f;

	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		SIGMA += 0.001f;

	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)
		SIGMA -= 0.001f;

	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		cam.incYaw(-cam.sensitivity);
		// cam.yaw -= cam.sensitivity;
		// cam.updateDirection();
	}

	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		cam.incYaw(cam.sensitivity);
		// cam.yaw += cam.sensitivity;
		// cam.updateDirection();
	}

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		// cam.pitch += cam.sensitivity;
		cam.incPitch(cam.sensitivity);
		// cam.updateDirection();
	}

	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		cam.incPitch(-cam.sensitivity);
		//cam.pitch -= cam.sensitivity;
		// cam.updateDirection();
	}
}

void mouse_callback(GLFWwindow* window, double xPos, double yPos) {
	// cam.handleMouse(xPos, yPos);
}