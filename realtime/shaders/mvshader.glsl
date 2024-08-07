#version 330 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normalV;
layout(location = 2) in vec2 tCoord;


uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec2 texCoord;
out vec3 normal;
out vec3 fragpos;


void main() {
	gl_Position = proj * view * model * vec4(pos.x, pos.y, pos.z, 1.0);
	fragpos = (model * vec4(pos, 1.0)).xyz;
	normal = mat3(transpose(inverse(model))) * normalV;
	texCoord = tCoord;
}