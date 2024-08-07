#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in VS_OUT{
	vec2 texCoord;
} gs_in[];

out vec2 texCoord;

uniform float time;

vec4 explode(vec4 position, vec3 normal) {
	float mag = 2.0f;
	vec3 dirToMove = normal * ((sin(time) + 1.0) / 2) * mag;
	return position + vec4(dirToMove, 0.0);
}

vec3 getNormal() {
	//the order of the cross product can be figure out using trial and error
	vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
	vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);

	return normalize(cross(a, b));
}

void main() {
	vec3 normal = getNormal();
	texCoord = gs_in[0].texCoord;
	gl_Position = explode(gl_in[0].gl_Position, normal);
	EmitVertex();

	texCoord = gs_in[1].texCoord;
	gl_Position = explode(gl_in[1].gl_Position, normal);
	EmitVertex();

	texCoord = gs_in[2].texCoord;
	gl_Position = explode(gl_in[2].gl_Position, normal);
	EmitVertex();

	EndPrimitive();



}