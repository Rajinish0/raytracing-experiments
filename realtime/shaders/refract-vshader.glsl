#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 normalC;

out vec3 fragpos;
out vec3 normal;


uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 transform;
void main()
{
	gl_Position = proj * view * model * transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);
	fragpos = (model * transform * vec4(aPos.x, aPos.y, aPos.z, 1.0)).xyz;
	/*
	* v.w = 0
	* or (v)'w = 0 (where v' is the transpose)
	* I want
	* (Av)'(Gw) = 0
	* v'A'Gw = 0
	* v'w = 0 so A'G should be I
	* A'G = I
	* G = (A')^-1
	* 
	* A'G = I
	* (G'A)' = I
	* (G'A)' = I
	* G' = A^-1
	* G = (A^-1)'
	* G = transp(inv(A))
	*/
	normal = mat3(transpose(inverse(model * transform))) * normalC;
}