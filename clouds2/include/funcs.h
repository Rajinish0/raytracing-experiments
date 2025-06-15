#ifndef FUNCS_GUARD
#define FUNCS_GUARD
#include <iostream>
#include <string>
#include "mesh.h"

namespace funcs {
	unsigned int TextureFromFile(const std::string& path, std::string directory);
	unsigned int loadCubeMap(std::vector<std::string> faces);
	Mesh genSphere();
	Mesh genTorus(); // or donut?
	template<typename T>
	T clamp(T val, T lowerb, T upperb){
		// return std::max(std::min(val, upperb), lowerb);
		if (val < lowerb) return lowerb;
		if (val > upperb) return upperb;
		return val;
	}
	GLuint genWorleyNoise(unsigned int width, unsigned int height, unsigned int breadth, unsigned int num_tiles = 10);
	GLuint loadWeatherData(std::string fname);
	GLuint loadDetailTexture(std::string fname);
	GLuint loadGeneric3dTexture(std::string fname);
	GLuint loadGeneric2dTexture(std::string fname);
}

#endif