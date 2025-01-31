#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 texCoord;


uniform mat4 proj;
uniform mat4 model;
uniform mat4 view;

out vec3 tCoord;

void main(){
    gl_Position = proj * view * model * vec4(aPos, 1.0f);
    tCoord = texCoord;
}