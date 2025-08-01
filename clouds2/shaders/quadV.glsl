#version 430 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 texCoord;

out vec2 tCoord;

void main(){
    gl_Position = vec4(aPos.x, aPos.y, 0.0f, 1.0f);
    tCoord = texCoord;
}