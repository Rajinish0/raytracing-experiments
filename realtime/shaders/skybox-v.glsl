#version 330 core

layout(location = 0) in vec3 pos;


out vec3 tCoords;

uniform mat4 proj;
uniform mat4 view;

void main() {
	tCoords = pos;
	vec4 p = proj * view * vec4(pos, 1.0);
	gl_Position = p.xyww;
}