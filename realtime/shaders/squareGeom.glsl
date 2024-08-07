#version 330 core

//layout(points) in;
//layout(points, max_vertices = 1) out;
//
//void main() {
//	gl_Position = gl_in[0].gl_Position;
//	EmitVertex();
//	EndPrimitive();
//}

/*
layout(points) in;
layout(line_strip, max_vertices = 2) out;

void main() {
	gl_Position = gl_in[0].gl_Position + vec4(-0.1, 0.0, 0.0, 0.0);
	EmitVertex();

	gl_Position = gl_in[0].gl_Position + vec4(0.1, 0.0, 0.0, 0.0);
	EmitVertex();

	EndPrimitive();

}
*/

layout(points) in;
/*
* given verts 1,2,3,4,5
* trianlge_strip is forming triangles like this
* (1,2,3) (2,3,4) (3,4,5)
*/
layout(triangle_strip, max_vertices = 5) out;

in VS_OUT{
	vec3 color;
}vs_in[]; // need a list because vertex shader passes the primitive it's drawing to the geometry shader
//so rn it's GL_POINTS so vs_in only has one point, for GL_TRIANGLES
// it would have 3 points corresponding to the vertices of the primitive

out vec3 fcol;

//kind of a pentagon
void build(vec4 pos) {
	fcol = vs_in[0].color;
	gl_Position = pos + vec4(-0.2, -0.2, 0.0, 0.0);
	EmitVertex();

	gl_Position = pos + vec4(0.2, -0.2, 0.0, 0.0);
	EmitVertex();

	gl_Position = pos + vec4(-0.2, 0.2, 0.0, 0.0);
	EmitVertex();

	gl_Position = pos + vec4(0.2, 0.2, 0.0, 0.0);
	EmitVertex();

	gl_Position = pos + vec4(0.0, 0.4, 0.0, 0.0);
	fcol = vec3(1.0);
	EmitVertex();

	EndPrimitive();
}

void main() {
	build(gl_in[0].gl_Position);
}