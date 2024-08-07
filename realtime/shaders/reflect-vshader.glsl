#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 normalC;
layout(location = 2) in vec2 inptexCoord;

//out vec2 texCoord;
out vec3 fragpos;
out vec3 normal;


uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 transform;
void main()
{
	gl_Position = proj * view * model * transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
	//texCoord = inptexCoord;
	fragpos = (model * transform * vec4(aPos.x, aPos.y, aPos.z, 1.0)).xyz;
	normal = mat3(transpose(inverse(model * transform))) * normalC;
}