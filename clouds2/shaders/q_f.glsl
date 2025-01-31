#version 430 core

in vec2 tCoord;

uniform sampler2D tex;

out vec4 fcol;

void main(){
    vec3 tCol = texture(tex, tCoord).rgb;
    fcol = vec4(tCol, 1.);
}