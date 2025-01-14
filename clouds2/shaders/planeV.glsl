#version 330 core

layout (location = 0) in vec2 aPos;

uniform mat4 proj;
uniform mat4 model;
uniform mat4 view;

const vec3 normal = vec3(0.0f, 0.0f, -1.0f);

out vec3 norm;
out vec3 worldPos;

void main(){
    vec4 worldP = model * vec4(aPos.x * 2, aPos.y * 2, 0.0, 1.0);
    worldPos = worldP.xyz;
    gl_Position = proj * view * worldP;
    norm = normalize((inverse(transpose(model)) * vec4(normal, 0.0)).xyz);
}