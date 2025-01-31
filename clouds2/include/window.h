#ifndef  WINDOW_HEADER
#define WINDOW_HEADER
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

struct Window {
	int W, H;
	GLFWwindow* window;

	Window(int w, int h)
		:W(w), H(h) {
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		window = glfwCreateWindow(w, h, "Learning opengl", NULL, NULL);
		if (window == NULL) {
			std::cerr << "Failed to create window" << std::endl;
			glfwTerminate();
		}

		glfwMakeContextCurrent(window);
	}

	bool shouldClose() {
		return glfwWindowShouldClose(window);
	}

	void update() {
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

};

#endif // ! WINDOW_HEADER