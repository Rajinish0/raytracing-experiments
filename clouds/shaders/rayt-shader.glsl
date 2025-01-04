#version 330 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 vCol;

out vec2 fragpos;
out vec3 col;

uniform mat4 model;
uniform mat4 view;


void main(){
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
    fragpos = aPos;
    col = vCol;
}