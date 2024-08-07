#version 330 core


in vec2 texCoord;
in vec3 normal;
in vec3 fragpos;

out vec4 FragColor;

uniform vec3 lightpos;
uniform vec3 lightcol;
uniform vec3 camPos;


struct Material {
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	float shininess;
};

struct Light {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

uniform Material material;
uniform Light light;
void main() {
	//FragColor = vec4(0.5, 0.3, 0.0, 1.0);
	FragColor = texture(material.texture_diffuse1, texCoord);

	//vec3 norm = normalize(normal);
	//vec3 diff = normalize(lightpos - fragpos);
	//float factor = max(dot(diff, norm), 0);
	//vec3 diffuse = light.diffuse * factor * vec3(texture(material.texture_diffuse1, texCoord));
	//vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, texCoord));

	//vec3 viewDir = normalize(camPos - fragpos);
	//float distance = length(lightpos - fragpos);
	//float attenuation = 1.0 / (light.constant + distance * (light.linear + distance * light.quadratic));
	////float attenuation = 1.0 / light.constant;

	////find reflection
	////vec3 vec = ( normal * dot(diff, normal) ) - diff;
	////vec3 ref = normalize(diff + 2 * vec);


	//vec3 ref = reflect(-diff, norm);

	//float spec = pow(max(dot(ref, viewDir), 0), material.shininess);
	//vec3 specular = light.specular * vec3(texture(material.texture_specular1, texCoord)) * spec;
	////ambient  *= attenuation;
	////diffuse  *= attenuation;
	////specular *= attenuation;
	//FragColor = vec4(diffuse + ambient + specular, 1.0f);

}