#version 330 core

uniform vec3 col;
in vec2 texCoord;
out vec4 fcol;

void main() {
	fcol = vec4(col, 1.0);
	//fcol = texture(texture_diff1, texCoord);
}