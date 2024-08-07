#version 330 core
layout(location = 0) in vec4 inVec;

uniform mat4 proj;

out vec2 tCoord;

void main() {
	gl_Position = proj * vec4(inVec.xy, 0.0, 1.0);
	tCoord = inVec.zw;
}