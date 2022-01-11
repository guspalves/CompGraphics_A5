//
// Created by Gus Alves on 11/27/21.
//

#ifndef A4_HERO_H
#define A4_HERO_H

#include <CSCI441/FreeCam.hpp>
#include <CSCI441/objects.hpp>
#include <CSCI441/OpenGLEngine.hpp>
#include <glm/glm.hpp>
#include <CSCI441/ShaderProgram.hpp>

class Hero {
public:
    Hero(CSCI441::ShaderProgram *shaderProgram, GLint modelLoc, GLint viewLoc, GLint projLoc, GLint colorLoc, glm::vec3 initPos);

    void draw(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx, GLfloat gunAngle);

    /// \desc movement functions
    void moveForward();
    void moveBackwards();
    void moveDown();
    void updateAngle(bool isLeft);

    /// \desc functions for lives of hero
    void decreaseLife();

    /// \desc getters
    glm::vec3 getLocation();
    glm::vec3 getDirection();
    GLint getLifesLeft();
    GLfloat getRadius();

    /// \desc setters
    void setDirection(glm::vec3 newDir);
    void setRadius(GLfloat newRadius);

private:
    /// \desc variables to store shader program
    CSCI441::ShaderProgram *_shaderProgram;
    struct locations {
        GLint model;
        GLint view;
        GLint proj;
        GLint color;
    }_shaderLocations;

    /// \desc variables to store information about the hero
    glm::vec3 _location;
    glm::vec3 _direction;
    GLfloat _rotAngle;
    GLfloat _speed;
    GLfloat _radius;
    GLint _numLifes;


    ///*****************************************************************************************************************
    /// Drawing Hero Functions
    ///*****************************************************************************************************************

    void _drawBody(glm::mat4 modelMtx);
    void _drawGun(glm::mat4 modelMtx, GLfloat gunAngle);
    void _drawRotationPiece(glm::mat4 modelMtx, GLfloat gunAngle);
};


#endif //A4_HERO_H
