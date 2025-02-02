#version 330 core

layout (location =0) in vec3 aPos;
layout (location =1) in vec3 normal;
layout (location =2) in vec2 texCoord;

// uniform float[10000] zCoords;

uniform sampler1D zCoordsTexture;


uniform int numVerts;
uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;
uniform vec4 planeNorm;

out vec3 col;
out vec2 tCoord;
out float dist;

void main(){
    float z = texture(zCoordsTexture, float(gl_VertexID) / float(numVerts)).r;
    vec4 worldPos = model*vec4(aPos.x, aPos.y, z, 1.0);
    // gl_ClipDistance[0] = dot(worldPos, planeNorm);
    vec4 viewPos = view * worldPos;
    // dist = abs(viewPos.z);
    dist = length(viewPos.xyz);
    gl_Position = proj*viewPos;
    tCoord = texCoord;
}