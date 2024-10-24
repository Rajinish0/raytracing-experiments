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
}

#endif