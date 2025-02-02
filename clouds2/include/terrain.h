#ifndef TERRAIN_H
#define TERRAIN_H

#include "shader.h"
#include "mesh.h"
#include "funcs.h"
#include "perlin.h"

struct Terrain{

    int numBlocks;
    float blockSize;
    Mesh myMesh;
    Shader& shader;
    Perlin2d perlin;
    std::vector<float> zCoords;
    float xOff, yOff;
    GLuint zCoordsTexture;
    Texture blendMap;
    

    Terrain(int numBlocks, float blockSize, Shader& shader)
        :numBlocks(numBlocks), blockSize(blockSize),
        myMesh(std::move(funcs::genPlane(numBlocks,
        blockSize, "textures/grass.jpg", 
        "textures/mud.png", 
        "textures/grassflowers.png",
        "textures/pavement.jpg"))),
        shader(shader), perlin(256, 8), xOff(1.0f), yOff(0.0f),
        blendMap("textures/blendmap.png", "texture_diffuse")
        {
            zCoords = std::vector<float>(numBlocks*numBlocks);
            glGenTextures(1, &zCoordsTexture);
            updateZCoords();
        }

    void updateZCoords(){
        float bsz = blockSize/(float(numBlocks - 1));
        float startx = -blockSize / 2.0f,
              starty =  blockSize / 2.0f,
              x, y;

        shader.use();
        for (int i =0; i < numBlocks; ++i){
            for (int j =0; j < numBlocks; ++j){
                // x = startx + bsz *j; 
                // y = starty - bsz *i;
                x = (float)j / (float)numBlocks;
                y = (float)i / (float)numBlocks;

                zCoords[funcs::flatten(i, j, numBlocks)] = 
                        std::max(-0.4f, perlin.perlin((xOff + x )*10.0f, 
                                      (yOff + y)*10.0f) * 40.0f );
            }
        }

        glBindTexture(GL_TEXTURE_1D, zCoordsTexture);
        glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, numBlocks * numBlocks, 0, GL_RED, GL_FLOAT, zCoords.data());
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        shader.use();
        shader.setInt("zCoordsTexture", 5);
        shader.setInt("numVerts", numBlocks * numBlocks);
        shader.setInt("blendMap", 4);
        shader.setFloat("tStretch", 40.0f);
        glBindTexture(GL_TEXTURE_1D, 0);
    }

    void move(float xd, float yd){
        if (xd == 0.0f && yd == 0.0f) return;
        xOff += xd; yOff += yd;
        updateZCoords();
    }

    void draw(){
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_1D, zCoordsTexture);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, blendMap.id);
        myMesh.draw(shader);
        glBindTexture(GL_TEXTURE_1D, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    ~Terrain(){
        // delete[] zCoords;
    }
};



#endif