#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 normalC;
layout(location = 2) in vec2 inptexCoord;

out vec2 texCoord;


uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
void main()
{
	gl_Position = proj * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
	texCoord = inptexCoord;
}