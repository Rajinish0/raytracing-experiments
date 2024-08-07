#version 330 core

in vec3 fragpos;
in vec3 normal;

uniform samplerCube texture_ref;
uniform vec3 camPos;

out vec4 fcol;

void main() {
	vec3 viewdir = normalize(camPos - fragpos);
	vec3 norm = normalize(normal);
	vec3 diff = norm * dot(viewdir, norm) - viewdir;
	vec3 ref = viewdir + 2 * diff;


	fcol = vec4(texture(texture_ref, ref).rgb, 1.0);
}