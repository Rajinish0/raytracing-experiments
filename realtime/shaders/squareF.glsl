#version 330 core

in vec3 fcol;
out vec4 fragcol;

void main() {
	fragcol = vec4(fcol, 1.0);
}