#version 330 core

uniform sampler2D texture_diff;
in vec2 texCoord;
out vec4 fragcol;

void main() {
	vec4 tCol = texture(texture_diff, texCoord);
	fragcol = tCol;
}