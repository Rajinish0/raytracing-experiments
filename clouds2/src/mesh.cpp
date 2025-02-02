#pragma once
#include "Mesh.h"
#include "funcs.h"
#include <exception>

Mesh::Mesh(std::vector<Vertex> verticies, std::vector<Texture> textures, std::vector<unsigned int> indicies)
	:verticies(verticies), textures(textures), indicies(indicies) {
	this->setUpMesh();
}


class InvalidTexture : public std::exception {
private :
	std::string message;
public:
	std::string what() {
		return this->message;
	}

	InvalidTexture(std::string msg)
		:message(msg) {}
};


Texture::Texture(std::string path, std::string directory, std::string type) {
	std::cout << "HERE now " << std::endl;
	this->id = funcs::TextureFromFile(path, directory);
	this->path = path;
	this->type = type;
}

Texture::Texture(std::string completePath, std::string type) {
	size_t i = completePath.find_last_of('/');
	std::string directory = completePath.substr(0, i);
	std::string path = completePath.substr(i+1, completePath.size());
	this->id = funcs::TextureFromFile(path, directory);
	this->path = path;
	this->type = type;
}

void Mesh::setUpMesh() {
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenVertexArrays(1, &VAO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, verticies.size() * sizeof(Vertex), &verticies[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicies.size() * sizeof(unsigned int), &indicies[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Mesh::draw(Shader& shader) {
	shader.use();
	int textureDiff = 0;
	int textureSpec = 0;
	for (int i = 0; i < textures.size(); ++i) {
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
		if (textures[i].type == "texture_diffuse") {
			shader.setInt("material." + textures[i].type + std::to_string(++textureDiff), i);
		}
		else if (textures[i].type == "texture_specular") {
			shader.setInt("material." + textures[i].type + std::to_string(++textureSpec), i);
		}
		else {
			throw InvalidTexture("Unrecognized texture type");
		}
	}

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indicies.size(), GL_UNSIGNED_INT, 0);
}
