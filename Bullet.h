//
// Created by Gus Alves on 11/26/21.
//

#ifndef A4_BULLET_H
#define A4_BULLET_H

#include <CSCI441/FreeCam.hpp>
#include <CSCI441/objects.hpp>
#include <CSCI441/OpenGLEngine.hpp>
#include <glm/glm.hpp>
#include <CSCI441/ShaderProgram.hpp>

class Bullet {
public:
    Bullet();

    /// \desc constructor that sets initial variables to user specified
    Bullet(glm::vec3 initPos, glm::vec3 direction);

    /// \desc draw function
    void draw(CSCI441::ShaderProgram *shaderProgram, GLint modelLoc, GLint viewLoc, GLint projLoc, GLint colorLoc, glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx, GLfloat rotAngle);

    /// \desc moves the bullet forward
    void moveForward();

    /// \desc getters
    glm::vec3 getLocation();
    GLfloat getRadius();

private:
    /// \desc variables for bullet attributes
    glm::vec3 _location;
    glm::vec3 _direction;
    const GLfloat SPEED = 1.0f;
    const GLfloat RADIUS = 1.0f;
};


#endif //A4_BULLET_H
