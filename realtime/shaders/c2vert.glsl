#version 330 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 tCoord;


layout (std140) uniform Matrices
{
	mat4 proj; //16 bytes for each vec4, 16*4 = 64 bytes
	mat4 view; // offset = 64 bytes, allocated bytes = 64 bytes
	// in total 128 bytes but just 2 * sizeof(glm::mat4);
};

uniform mat4 model;

out vec2 texCoord;

void main() {
	gl_Position = proj * view * model * vec4(pos, 1.0);
	texCoord = tCoord;
}