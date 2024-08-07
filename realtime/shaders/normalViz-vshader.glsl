#version 330 core
layout(location = 0) in vec3 aPos;
//layout (location = 1) in vec3 color;
layout(location = 1) in vec3 normalC;
layout(location = 2) in vec2 inptexCoord;

//out vec4 vCol;
out vec2 texCoord;
out vec3 fragpos;

out VS_OUT{
	vec3 normal;
} vs_out;


uniform mat4 model;
uniform mat4 view;
//uniform mat4 proj;
uniform mat4 transform;


void main()
{
	gl_Position = view * model * transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
	texCoord = inptexCoord;
	fragpos = (model * transform * vec4(aPos.x, aPos.y, aPos.z, 1.0)).xyz;
	vs_out.normal = normalize(mat3(transpose(inverse(view * model * transform))) * normalC);
}