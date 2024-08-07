#version 330 core

out vec4 fragcol;

in vec3 tCoords;

uniform samplerCube skybox;



void main() {
	//fragcol = vec4(1.0, 0.0, 0.0, 1.0);
	fragcol = texture(skybox, tCoords);
}