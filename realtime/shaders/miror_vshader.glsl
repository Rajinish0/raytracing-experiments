#version 330 core
layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 tCoord;

out vec2 texCoord;


uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
	gl_Position = model*vec4(pos.x, pos.y, 0.0, 1.0);
	texCoord = tCoord;
}