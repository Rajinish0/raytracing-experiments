#version 330 core

in vec2 tCoord;
out vec4 fcol;

uniform sampler2D text;
uniform vec3 textColor;


void main() {
	float alphaVal = texture(text, tCoord).r;
	fcol = vec4(textColor, alphaVal);
}