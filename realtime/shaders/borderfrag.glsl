#version 330 core

uniform sampler2D texture_diff;
in vec2 texCoord;
out vec4 fcol;

void main() {
	fcol = vec4(1.0, 1.0, 0.5, 0.3);
	//fcol = texture(texture_diff, texCoord);
}