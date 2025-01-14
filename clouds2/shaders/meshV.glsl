#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 tCoord;


uniform mat4 proj;
uniform mat4 model;
uniform mat4 view;

out vec3 norm;
out vec3 worldPos;

void main(){
    vec4 worldP = model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
    worldPos = worldP.xyz;
    gl_Position = proj * view * worldP;
    norm = normal;
}