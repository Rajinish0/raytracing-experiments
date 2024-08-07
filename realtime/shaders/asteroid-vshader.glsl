#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 normalC;
layout(location = 2) in vec2 inptexCoord;
layout(location = 3) in mat4 instanceModelMat;

out vec2 texCoord;


uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
void main()
{
	gl_Position = proj * view * instanceModelMat * vec4(aPos.x, aPos.y, aPos.z, 1.0);
	texCoord = inptexCoord;
}