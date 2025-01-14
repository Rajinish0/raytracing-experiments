#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "shader.h"
#include "plane.h"

class FrameBuffer{
private:
    bool isReady = false;
    Plane plane;
    glm::vec4 clearColor = glm::vec4(0.86f, 0.82f, 0.78f, 1.0f);

public:
    unsigned int FBO, RBO, textureId, depthTextureId;

    FrameBuffer(){
        glGenFramebuffers(1, &FBO);
        glGenRenderbuffers(1, &RBO);

        glBindFramebuffer(GL_FRAMEBUFFER, FBO);

        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId, 0);

        glGenTextures(1, &depthTextureId);
        glBindTexture(GL_TEXTURE_2D, depthTextureId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, 800, 600, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTextureId, 0);

        // glBindRenderbuffer(GL_RENDERBUFFER, RBO);
        // glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 800, 600);
        // glBindRenderbuffer(GL_RENDERBUFFER, 0);

        // glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

        // glGenTextures(1, &depthTextureId);
        // glBindTexture(GL_TEXTURE_2D, depthTextureId);

        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
        else 
            isReady = true;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Bind() const{
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
		glEnable(GL_DEPTH_TEST);
		glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void unBind() const{
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void draw(Shader& shader) {
        // glDisable(GL_DEPTH_TEST);
        plane.draw(shader, textureId, depthTextureId);
        // glEnable(GL_DEPTH_TEST);
    }

    void setClearColor(glm::vec4 color){
        this->clearColor = color;
    }


};