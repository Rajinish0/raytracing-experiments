#ifndef MODEL_HEADER
#define MODEL_HEADER

#include "mesh.h"
#include "funcs.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


class Model {
public:
	Model(std::string path);
	void draw(Shader& shader);
	std::vector<Mesh> meshes;

private:
	std::string directory;

	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> cachedTextures;
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};



#endif

