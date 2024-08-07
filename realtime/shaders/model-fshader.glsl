#version 330 core

in vec2 texCoord;
out vec4 fcol;

uniform sampler2D texture_diffuse1;

void main()
{
	fcol = texture(texture_diffuse1, texCoord);
}