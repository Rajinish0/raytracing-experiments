#version 330 core

in vec3 col;
out vec4 fragcol;

const vec3 skycolor = vec3(0.86f, 0.82f, 0.78f);
const float maxDist = 10.0f;

in vec2 tCoord;
in float dist;

uniform float tStretch;
uniform sampler2D blendMap;
uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_diffuse4;

vec3 applyFog(in vec3 col, in float t){
    float fogAmt = 1.0 - exp(-t*0.3f);
    // vec3 fogColor = vec3(0.5, 0.6, 0.7);
    return mix(col, skycolor, fogAmt);
}


void main(){
    vec3 blend = texture(blendMap, tCoord).rgb;
    float t1fac = 1 - (blend.r + blend.g + blend.b);
    vec3 t1 = texture(texture_diffuse1, tCoord*tStretch).rgb;
    vec3 t2 = texture(texture_diffuse2, tCoord*tStretch).rgb;
    vec3 t3 = texture(texture_diffuse3, tCoord*tStretch).rgb;
    vec3 t4 = texture(texture_diffuse4, tCoord*tStretch).rgb;

    vec3 tCol = t1fac*t1 + blend.r * t2 + blend.g * t3 + blend.b * t4;

    float d = min(dist, maxDist);
    float alpha = d/maxDist;
    vec3 tempcol = (1-alpha)*tCol + (alpha)*skycolor;
    // vec3 tempcol = applyFog(tCol, d);

    fragcol = vec4(vec3(0.60784, 0.43529, 0.31372), 1.0f);
    // fragcol = vec4(col, 1.0f);
    // fragcol = vec4(1.0f, 0.5f, 0.0f, 1.0f);
}