//
// Created by Gus Alves on 11/26/21.
//

#include "Bullet.h"
Bullet::Bullet(){
    _location = glm::vec3(0, 0, 0);
    _direction = glm::vec3(0.1f, 0.0f, 0.1f);
}

Bullet::Bullet(glm::vec3 initPos, glm::vec3 direction){
    _location = initPos;
    _direction = direction;
}

void Bullet::draw(CSCI441::ShaderProgram *shaderProgram, GLint modelLoc, GLint viewLoc, GLint projLoc, GLint colorLoc,
                  glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx, GLfloat rotAngle) {

    shaderProgram->useProgram();
    glm::mat4 tempModel = glm::translate(modelMtx, _location);
    glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f);

    shaderProgram->setProgramUniform(modelLoc, tempModel);
    shaderProgram->setProgramUniform(viewLoc, viewMtx);
    shaderProgram->setProgramUniform(projLoc, projMtx);
    shaderProgram->setProgramUniform(colorLoc, color);

    CSCI441::drawSolidSphere(RADIUS, 16, 16);

    shaderProgram->setProgramUniform(modelLoc, modelMtx);
}

void Bullet::moveForward() {
    _location += (SPEED * _direction);
}

/// \desc getters
glm::vec3 Bullet::getLocation(){ return _location; }
GLfloat Bullet::getRadius(){ return RADIUS; }