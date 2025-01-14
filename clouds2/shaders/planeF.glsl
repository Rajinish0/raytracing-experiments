#version 330 core

out vec4 fcol;
uniform vec3 lightPos;

in vec3 norm;
in vec3 worldPos;


void main(){
    vec3 col = vec3(0.3f, 0.1f, 0.1);
    vec3 lightDir = normalize(lightPos - worldPos);
    float diff = max(dot(lightDir, norm), 0.0);
    fcol = vec4(col * diff, 1.0f);
}