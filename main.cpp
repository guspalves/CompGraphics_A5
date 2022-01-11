/*
 *  CSCI 441, Computer Graphics, Fall 2021
 *
 *  Project: lab04
 *  File: main.cpp
 *
 *  Description:
 *      This file contains the basic setup to work with GLSL shaders.
 *
 *  Author: Dr. Paone, Colorado School of Mines, 2021
 *
 */

#include "A5Engine.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

///*****************************************************************************
//
// Our main function
int main() {

    auto assignmentEngine = new A5Engine(4, 1, 640, 640, "A5 - The Barbarian Horde");
    assignmentEngine->initialize();
    if (assignmentEngine->getError() == CSCI441::OpenGLEngine::OPENGL_ENGINE_ERROR_NO_ERROR) {
        assignmentEngine->run();
    }
    assignmentEngine->shutdown();
    delete assignmentEngine;

	return EXIT_SUCCESS;
}
