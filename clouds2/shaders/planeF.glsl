#version 330 core

out vec4 fcol;
uniform vec3 lightPos;
uniform sampler2D myTexture;

in vec3 norm;
in vec3 worldPos;
in vec2 tCoord;


void main(){
    vec3 col = vec3(0.3f, 0.1f, 0.1);
    vec3 lightDir = normalize(lightPos - worldPos);
    float diff = max(dot(lightDir, norm), 0.0);
    fcol = vec4(col * diff, 1.0f);
    // fcol = vec4(vec3(texture(myTexture, tCoord).r), 1.0);
}