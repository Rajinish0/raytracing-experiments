#version 330 core

layout(location = 0) in vec2 pos;
layout(location = 1) in vec3 vcol;

out vec3 color;

uniform vec2 offsets[100];

void main() {
	vec2 offset = offsets[gl_InstanceID]; // istanceID starts at 0
	gl_Position = vec4(pos/10.0 + offset, 0.0, 1.0);
	color = vcol;
}