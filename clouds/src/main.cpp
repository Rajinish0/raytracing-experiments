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


float noise[800 * 600];
Perlin2d perlin;


unsigned int quadVBO, quadVAO;
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

	glEnable(GL_DEPTH_TEST);

	glGenBuffers(1, &quadVBO);

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
	proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 1.0f, 500.0f);
	invProj = glm::inverse(proj);
	invView = glm::inverse(view);



	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	float ang = 0.0f;
	// glm::vec3 spPos[] = {
	// 	glm::vec3(-1.0f, 0.0f, -27.75f),
	// 	glm::vec3( 30.0f, 2.0f, -29.75f),
	// };
	std::vector<glm::vec3> spPos {
		glm::vec3(0.0f, 0.0, 8.1),
		glm::vec3(-5.0f, 0.0f, 8.1)
	};
	// for (int i = 0; i < 50; i++){
	// 	spPos.push_back(
	// 		glm::vec3(  randNum(-50.0f, 50.0f),
	// 					0.0f,
	// 					// randNum(-5.0f, 5.0f), 
	// 				 	randNum(-80.0f, -30.0f )
	// 			)
	// 	);
	// }
	for (int a = -5; a < 5; a++){
		for (int b = -3; b < 2; b++){
			spPos.push_back(
				glm::vec3(
					a * 10.0f,
					0.0f, 
					b * 10.0f
				)
			);
		}
	}

	rayShdr.use();
	rayShdr.setInt("numSpheres", 52);
	for (int i = 0; i < 2; ++i){
		rayShdr.setVec3("mySphere["+ std::to_string(i) +"].pos", spPos[i] );
		rayShdr.setVec3("mySphere["+ std::to_string(i) +"].col", glm::vec3(1.0f, 1.0f, 0.0f));
		rayShdr.setFloat("mySphere["+ std::to_string(i) +"].radius", 10.0f);
		rayShdr.setInt("mySphere["+ std::to_string(i) +"].type", OPAQUE_T);
		rayShdr.setInt("mySphere["+ std::to_string(i) +"].textType", FROM_COLOR);
	}
	rayShdr.setInt("mySphere[1].textType", FROM_TEXTURE);
	rayShdr.setInt("mySphere[0].type", REFRACT_T);
	rayShdr.setFloat("mySphere[0].ior", 1.50f);

	rayShdr.setVec3("mySphere["+ std::to_string(2) +"].pos", spPos[0] );
	rayShdr.setVec3("mySphere["+ std::to_string(2) +"].col", glm::vec3(1.0f, 1.0f, 1.0f));
	rayShdr.setFloat("mySphere["+ std::to_string(2) +"].radius", 8.0f);
	rayShdr.setInt("mySphere["+ std::to_string(2) +"].type", REFRACT_T);
	rayShdr.setInt("mySphere["+ std::to_string(2) +"].textType", FROM_COLOR);
	rayShdr.setFloat("mySphere[2].ior", 1.0f/1.50f);



	for (int i =3; i <52; i++){
		rayShdr.setVec3("mySphere["+ std::to_string(i) +"].pos", spPos[i] );
		rayShdr.setVec3("mySphere["+ std::to_string(i) +"].col", 
						glm::vec3(randNum(0.0f, 1.0f), randNum(0.0f, 1.0f), 0.3f));
		float rad = randNum(1.0f, 6.0f);
		rayShdr.setFloat("mySphere["+ std::to_string(i) +"].radius", rad );
		int type = OPAQUE_T;
		float randN = randNum(0.0f, 1.0f);
		if (randN < 0.33f)
			type = REFLECT_T;
		else if (randN < 0.66)
			type = REFRACT_T;
		
		rayShdr.setInt("mySphere["+ std::to_string(i) +"].type", type);
		rayShdr.setInt("mySphere["+ std::to_string(i) +"].textType", FROM_COLOR);

		if (type == REFRACT_T){
			rayShdr.setFloat("mySphere[" + std::to_string(i) +"].ior", 1.50f);
			i++;
			rayShdr.setVec3("mySphere["+std::to_string(i)+"].pos", spPos[i-1]);
			rayShdr.setVec3("mySphere["+ std::to_string(i) +"].col", glm::vec3(1.0f));
			rayShdr.setFloat("mySphere["+ std::to_string(i) +"].radius", rad*0.8f);
			rayShdr.setInt("mySphere["+ std::to_string(i) +"].type", type);
			rayShdr.setInt("mySphere["+ std::to_string(i) +"].textType", FROM_COLOR);
			rayShdr.setFloat("mySphere["+std::to_string(i)+"].ior",  1.0f/1.50f);
		}
	}

	rayShdr.setVec3("mySphere["+std::to_string(52)+"].pos", glm::vec3(0.0f, -1050.0f, -100.0f));
	rayShdr.setVec3("mySphere["+ std::to_string(52) +"].col", glm::vec3(0.5f, 0.5f, 0.5f));
	rayShdr.setFloat("mySphere["+ std::to_string(52) +"].radius", 1000.0f);
	rayShdr.setInt("mySphere["+ std::to_string(52) +"].type", OPAQUE_T);
	rayShdr.setInt("mySphere["+ std::to_string(52) +"].textType", FROM_COLOR);
	// rayShdr.setFloat("mySphere["+std::to_string(52)+"].ior",  1.0f/1.50f);


	
	rayShdr.setVec3("camPos", cam.position);
	rayShdr.setMatrix("invProjMat", invProj);
	// rayShdr.setVec3("lightPos", lightPos);
	cam.position = glm::vec3(0.0f, 0.0f, 5.0f);
	cam.incPitch(-25.0f);
	// rayShdr.setMatrix("invViewMat", invView);


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	std::vector<float> noiseData(800 * 600 * 2);
	srand(static_cast<unsigned int>(time(0)));
	for (int i =0; i < 600; ++i){
		for (int j =0; j < 800; ++j){
			float nx = static_cast<float>(j) / 800.0f;
			float ny = static_cast<float>(j) / 600.0f;
			// float nv  = perlin.perlin(nx, ny);
			// float nv2 = perlin.perlin(nx + 100.0, ny + 100.0);
			float nv = static_cast<float>(rand())/static_cast<float>(RAND_MAX);
			float nv2 = static_cast<float>(rand())/static_cast<float>(RAND_MAX);
			noiseData[2*(i * 800 + j)] = (nv);
			noiseData[2*(i * 800 + j) + 1] = (nv2);
			// noiseData[2 * (i * 800 + j)] = perlin.perlin()
		}
	}

	// stbi_write_png("uNoise2D.png", 800, 600, 1, noiseData.data(), 800);
	// updateNoise();

	unsigned int tId; 
	glGenTextures(1, &tId);
	glBindTexture(GL_TEXTURE_2D, tId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG, 800, 600, 0,
	GL_RG, GL_FLOAT, noiseData.data());

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// glEnable(GL_BLEND);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// glBlendEquation(GL_FUNC_ADD);

	cam.sensitivity *= 4.0;


	while (!window.shouldClose())
	{

		
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		processInput(window.window);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		view = cam.getView();

		rayShdr.use();
		glBindTexture(GL_TEXTURE_CUBE_MAP, skboxTexture);

		rayShdr.setInt("texture_diffuse1", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture2);

		rayShdr.setInt("uNoise", 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tId);

		rayShdr.setFloat("uTime", (float)glfwGetTime());


		// rayShdr.setMatrix("invViewMat", glm::inverse(view));
		ang += 0.001f;
		rayShdr.setVec3("camPos", cam.position);
		glm::vec3 p = spPos[1];
		// glm::mat4 m = (glm::rotate(glm::mat4(1.0f), ang, glm::vec3(0.0f, 1.0f, 0.0f)));
		// p = glm::vec3(glm::vec4(p, 1.0) * m);
		p.x *= glm::cos((float)glfwGetTime()) * 4;
		// rayShdr.setVec3("mySphere[1].pos", p);
		rayShdr.setMatrix("sphereModel", glm::mat4(1.0f));
		// rayShdr.setMatrix("sphereModel", (glm::rotate(glm::mat4(1.0f), ang*10, glm::vec3(0.0f, 1.0f, 0.0f))));
		rayShdr.setMatrix("invViewMat", glm::inverse(view) );
		// glm::mat4 rot = glm::rotate(
		// 						glm::mat4(1.0f),
		// 						(float)glfwGetTime() * 2.0f,
		// 						glm::vec3(1.0f, 0.0f, 0.0f)
		// 					);

		// rayShdr.setVec3("lightPos", 
		// 				glm::vec3(
		// 					rot * glm::vec4(lightPos, 1.0) 
		// 				)
		// 			);
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		/*
		instancingShader.use();
		glBindVertexArray(squareVAO);
		//glDrawArrays(GL_TRIANGLES, 0, 6);
		glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);
		*/

		// pShader.use();
		// pShader.setMatrix("proj", proj);
		// pShader.setMatrix("view", view);
		// pShader.setMatrix("model", glm::mat4(1.0f));
		// planet.draw(pShader);


		// aShader.use();
		// aShader.setMatrix("proj", proj);
		// aShader.setMatrix("view", view);
		// for (unsigned int i = 0; i < asteriod.meshes.size(); ++i) {
		// 	glBindVertexArray(asteriod.meshes[i].VAO);
		// 	glDrawElementsInstanced(
		// 		GL_TRIANGLES, asteriod.meshes[i].indicies.size(), GL_UNSIGNED_INT, 0, amt
		// 	);
		// }


		//glm::mat4 myMat(1.0f);
		//myMat = glm::rotate(myMat, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));

		//for (int i = 0; i < amt; ++i) {
		//	pAndAShader.setMatrix("model", myMat*modelMats[i]);
		//	asteriod.draw(pAndAShader);
		//}

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
		cam.move(Camera::UP, dt);

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cam.move(Camera::DOWN, dt);

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cam.move(Camera::RIGHT, dt);

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cam.move(Camera::LEFT, dt);

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