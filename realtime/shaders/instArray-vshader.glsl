#version 330 core

layout(location = 0) in vec2 pos;
layout(location = 1) in vec3 vcol;
layout(location = 2) in vec2 offset; // we can change the time that this
/*
* changes to the next value in the buffer using glVertexAttribDivisor(2, x)
* x = 0 --> every vertex
* x = 1 --> after one instance
* x = 2 --> after two instances
* ..
*/

out vec3 color;

void main() {
	gl_Position = vec4( (pos / 10.0 ) * (gl_InstanceID / 100.0) + offset, 0.0, 1.0);
	color = vcol;
}