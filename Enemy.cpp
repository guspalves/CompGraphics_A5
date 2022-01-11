//
// Created by Gus Alves on 11/25/21.
//

#include <iostream>
#include "Enemy.h"

Enemy::Enemy(){}

Enemy::Enemy(glm::vec3 initPos, glm::vec3 direction, GLfloat radius){
    _rotation = 0.0f;
    _location = initPos;
    _direction = direction;
    _radius = radius;
    _color = glm::vec4(237.0f/255.0f, 38.0f/255.0f, 45.0f/255.0f, 1.0f);
}
/// ********************************************************************************************************************
/// drawing functions
/// ********************************************************************************************************************


void Enemy::draw(CSCI441::ShaderProgram *shaderProgram, GLint mvpLoc, GLint colorUniformLocation, glm::mat4 modelMtx,
                 glm::mat4 projViewMtx) {
    glm::vec3 rotationAxis = glm::cross( _direction, CSCI441::Y_AXIS );

    modelMtx = glm::translate( modelMtx, _location );
    modelMtx = glm::translate( modelMtx, glm::vec3(0, _radius, 0 ) );
    modelMtx = glm::rotate( modelMtx, _rotation, rotationAxis );
    glm::mat4 mvpMatrix = projViewMtx * modelMtx;
    shaderProgram->setProgramUniform( mvpLoc, mvpMatrix );

    shaderProgram->setProgramUniform(colorUniformLocation, _color );

    CSCI441::drawSolidSphere(_radius, 16, 16 );
}

/// ********************************************************************************************************************
/// movement functions
/// ********************************************************************************************************************

void Enemy::moveForward() {
    _location = _location + (_direction * _speed);
    _rotation -= _speed;
    if( _rotation < 0.0f ) {
        _rotation += 6.28f;
    }

}

void Enemy::moveBackwards() {
    _location = _location - (_direction * _speed);
    _rotation += _speed;
    if( _rotation > 6.28f ) {
        _rotation -= 6.28f;
    }
}

/// ********************************************************************************************************************
/// gameplay functions
/// ********************************************************************************************************************

void Enemy::doubleSpeed() {
    _speed *= 2;
}

/// ********************************************************************************************************************
/// setters & getters
/// ********************************************************************************************************************

void Enemy::setDirection(glm::vec3 newDir) { _direction = newDir; }
void Enemy::setSpeed(GLfloat newSpeed) { _speed = newSpeed; }
void Enemy::setRadius(GLfloat newRadius) { _radius = newRadius; }
glm::vec3 Enemy::getDirection() { return _direction; }
glm::vec3 Enemy::getLocation() { return _location; }
GLfloat Enemy::getRadius(){ return _radius; }
GLfloat Enemy::getSpeed(){ return _speed; }

