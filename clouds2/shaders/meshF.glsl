#version 430 core


out vec4 fcol;
in vec3 norm;
in vec3 worldPos;


uniform vec3 lightPos;

uniform bool isLight;

// uniform vec3 lightPos;


void main(){
    vec4 col = vec4(0.3, 0.3, 0.7, 1.0f);
    vec3 lightDir = normalize(lightPos - worldPos);
    float diff = max(dot(lightDir, norm), 0.0);

    if (!isLight)
        fcol = vec4(col.xyz * diff, 1.0f);
    else 
        fcol = vec4(1.0f);
}