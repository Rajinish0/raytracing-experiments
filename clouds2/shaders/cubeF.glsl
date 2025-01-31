#version 430 core

in vec3 tCoord;

uniform sampler3D texture_diffuse;

out vec4 fcol;

void main(){
    fcol = vec4(vec3(texture(texture_diffuse, tCoord).r), 1.0);
}