/*
 *   Vertex Shader
 *
 *   CSCI 441, Computer Graphics, Colorado School of Mines
 */

#version 410 core

// all uniform inputs
uniform mat4 modelMtx;
uniform mat4 viewMtx;
uniform mat4 projMtx;

uniform vec3 matColor;

// all attribute inputs
in vec3 vPosition;

// all varying outputs
out vec4 color;

void main() {
    //*****************************************
    //********* Vertex Calculations  **********
    //*****************************************
    gl_Position = projMtx * viewMtx * modelMtx * vec4(vPosition, 1.0);

    color = vec4(matColor, 1.0);
}
