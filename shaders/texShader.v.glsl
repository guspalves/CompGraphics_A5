#version 410 core

// uniform inputs
uniform mat4 mvpMatrix;

// attribute inputs
in vec3 vPos;
// TODO #A
in vec2 vTexCoord;

// varying outputs
// TODO #B
out vec2 texCoord;

void main() {
    gl_Position = mvpMatrix * vec4(vPos, 1.0);

    // TODO #C
    texCoord = vTexCoord;
}