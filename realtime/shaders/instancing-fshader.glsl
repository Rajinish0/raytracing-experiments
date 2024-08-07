#version 330 core

in vec3 color;
out vec4 fcol;

void main() {
	fcol = vec4(color, 1.0);
}