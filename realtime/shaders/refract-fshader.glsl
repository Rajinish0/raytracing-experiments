#version 330 core

in vec3 fragpos;
in vec3 normal;

uniform samplerCube texture_ref;
uniform vec3 camPos;

out vec4 fcol;

void main() {
	//fcol = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	
	float ratio = 1.0 / 1.52; // (air / glass)
	vec3 viewdir = normalize(fragpos - camPos);
	vec3 norm = normalize(normal);
	//snell's law 
	/*ss
	* TO DO:
	*	derive vector equation for this.
	*/
	vec3 refvec = refract(viewdir, norm, ratio);

	fcol = vec4(texture(texture_ref, refvec).rgb, 1.0);
	
}