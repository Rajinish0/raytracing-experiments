#ifndef MESH_HEADER
#define MESH_HEADER 


#include <iostream>
#include <string>
#include <vector>
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoord;

};

struct Texture {
	unsigned int id;
	std::string type;
	std::string path;

	Texture() 
		:id(0) {};
	Texture(unsigned int id, std::string type, std::string path)
		:id(id), type(type), path(path) {}
	Texture(std::string path, std::string directory, std::string type);
	Texture(std::string completePath, std::string type);
};


class Mesh {
public:
	std::vector<Vertex> verticies;
	std::vector<Texture> textures;
	std::vector<unsigned int> indicies;
	
	Mesh(std::vector<Vertex> verticies, std::vector<Texture> textures, std::vector<unsigned int> indicies);
	void draw(Shader& shader);
	unsigned int VAO, VBO, EBO;

private:
	void setUpMesh();
};

#endif