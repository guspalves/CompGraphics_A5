//
// Created by Gus Alves on 11/25/21.
//

#ifndef A4_ENEMY_H
#define A4_ENEMY_H

#include <CSCI441/FreeCam.hpp>
#include <CSCI441/objects.hpp>
#include <CSCI441/OpenGLEngine.hpp>
#include <CSCI441/ShaderProgram.hpp>
#include <CSCI441/materials.hpp>
#include <vector>
#include <glm/glm.hpp>

class Enemy {
public:
    /// \desc base constructor
    Enemy();

    /// \desc constructor that sets the initial pos
    Enemy(glm::vec3 initPos, glm::vec3 direction, GLfloat radius);

    /// \desc draw the enemy
    void draw(CSCI441::ShaderProgram *shaderProgram, GLint mvpLoc, GLint colorUniformLocation, glm::mat4 modelMtx, glm::mat4 projViewMtx);

    /// \decs set a new direction
    /// @param newDir: new direction of the enemy (must be normalized)
    void setDirection(glm::vec3 newDir);

    /// \desc getters
    glm::vec3 getLocation();
    glm::vec3 getDirection();
    GLfloat getRadius();
    GLfloat getSpeed();

    /// \desc setters
    void setSpeed(GLfloat newSpeed);
    void setRadius(GLfloat newRadius);

    /// \desc movement functions
    void moveForward();
    void moveBackwards();

    /// \desc double the speed of the enemies
    void doubleSpeed();

private:
    /// \desc variables used for enemy creation
    glm::vec3 _location;
    glm::vec3 _direction;

    /// \desc variable to store how much the enemy moves per frame
    GLfloat _speed = 0.1f;

    /// \desc variable to store the color
    glm::vec4 _color;

    /// \desc variable to store the radius of the enemy
    GLfloat _radius;

    /// \desc variable to store current rotation
    GLfloat _rotation;
};


#endif //A4_ENEMY_H
