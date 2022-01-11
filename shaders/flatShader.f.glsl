/*
 *   Fragment Shader
 *
 *   CSCI 441, Computer Graphics, Colorado School of Mines
 */

#version 410 core

// all uniform inputs
in vec4 color;

// all fragment outputs
out vec4 fragColorOut;

void main() {
    //*****************************************
    //******* Final Color Calculations ********
    //*****************************************
    
    fragColorOut = color;
}
