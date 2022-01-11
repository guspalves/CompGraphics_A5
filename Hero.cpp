//
// Created by Gus Alves on 11/27/21.
//

#include "Hero.h"

Hero::Hero(CSCI441::ShaderProgram *shaderProgram, GLint modelLoc, GLint viewLoc, GLint projLoc, GLint colorLoc, glm::vec3 initPos){
    _shaderProgram = shaderProgram;
    _shaderLocations.model = modelLoc;
    _shaderLocations.view = viewLoc;
    _shaderLocations.proj = projLoc;
    _shaderLocations.color = colorLoc;

    _speed = 1.0f;
    _rotAngle = 0;
    _numLifes = 3;
    _radius = 3;

    _location = initPos;
    _direction = glm::vec3(sinf(_rotAngle), 0, cosf(_rotAngle));
}

///*********************************************************************************************************************
/// Drawing functions
///*********************************************************************************************************************

void Hero::draw(glm::mat4 modelMtx, glm::mat4 viewMtx, glm::mat4 projMtx, GLfloat gunAngle){
    _shaderProgram->useProgram();

    modelMtx = glm::translate(modelMtx, _location);
    modelMtx = glm::rotate(modelMtx, _rotAngle, CSCI441::Y_AXIS);

    _shaderProgram->setProgramUniform(_shaderLocations.model, modelMtx);
    _shaderProgram->setProgramUniform(_shaderLocations.view, viewMtx);
    _shaderProgram->setProgramUniform(_shaderLocations.proj, projMtx);

    /// drawing body
    _drawBody(modelMtx);
    _drawGun(modelMtx, gunAngle);
    _drawRotationPiece(modelMtx, gunAngle);
}

void Hero::_drawBody(glm::mat4 modelMtx){
    glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f);

    _shaderProgram->setProgramUniform(_shaderLocations.color, color);
    _shaderProgram->setProgramUniform(_shaderLocations.model, modelMtx);

    CSCI441::drawSolidSphere(_radius, 16, 16);
}

void Hero::_drawGun(glm::mat4 modelMtx, GLfloat gunAngle){
    glm::mat4 tempModel = glm::rotate(modelMtx, (GLfloat) M_PI/2.0f, CSCI441::X_AXIS);
    glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

    _shaderProgram->setProgramUniform(_shaderLocations.model, tempModel);
    _shaderProgram->setProgramUniform(_shaderLocations.color, color);

    CSCI441::drawSolidCone(2.0f, 6.0f, 16, 16);
}

void Hero::_drawRotationPiece(glm::mat4 modelMtx, GLfloat gunAngle){
    for(int i = 1; i <= 50; i++) {
        glm::mat4 tempModel = glm::rotate(modelMtx, (GLfloat)(i * 2.0f*M_PI/50.0f), CSCI441::X_AXIS);
        tempModel = glm::rotate(tempModel, gunAngle, CSCI441::X_AXIS);
        glm::vec3 color = glm::vec3(56.0f/255.0f, 140.0f/255.0f, 242.0f/255.0f);

        _shaderProgram->setProgramUniform(_shaderLocations.model, tempModel);
        _shaderProgram->setProgramUniform(_shaderLocations.color, color);

        CSCI441::drawSolidCone(1.0f, 5.0f, 16, 16);
    }

    for(int i = 1; i <= 50; i++) {
        glm::mat4 tempModel = glm::rotate(modelMtx, (GLfloat)(M_PI/2.0f), CSCI441::X_AXIS);
        tempModel = glm::rotate(tempModel, (GLfloat)(i * 2.0f * M_PI/50.0f), CSCI441::Z_AXIS);
        tempModel = glm::rotate(tempModel, gunAngle, CSCI441::Z_AXIS);
        glm::vec3 color = glm::vec3(56.0f/255.0f, 140.0f/255.0f, 242.0f/255.0f);

        _shaderProgram->setProgramUniform(_shaderLocations.model, tempModel);
        _shaderProgram->setProgramUniform(_shaderLocations.color, color);

        CSCI441::drawSolidCone(1.0f, 5.0f, 16, 16);
    }
}

///*********************************************************************************************************************
/// Movement Functions
///*********************************************************************************************************************

void Hero::moveForward(){
    _location += _speed * _direction;
}

void Hero::moveBackwards(){
    _location -= _speed * _direction;
}

void Hero::moveDown(){
    _location = glm::vec3(_location.x, _location.y - 4.0f, _location.z);
}

void Hero::updateAngle(bool isLeft){
    if(isLeft){
        _rotAngle += M_PI / 60.0;
    } else {
        _rotAngle -= M_PI / 60.0;
    }

    _direction = glm::vec3(sinf(_rotAngle), 0, cosf(_rotAngle));
}

///*********************************************************************************************************************
/// Gameplay function
///*********************************************************************************************************************

void Hero::decreaseLife(){
    _numLifes--;
}

///*********************************************************************************************************************
/// Getters
///*********************************************************************************************************************

glm::vec3 Hero::getLocation() { return _location; }
glm::vec3 Hero::getDirection(){ return glm::vec3(sinf(_rotAngle), 0, cosf(_rotAngle)); }
GLint Hero::getLifesLeft(){ return _numLifes; }
GLfloat Hero::getRadius(){ return _radius; }

///*********************************************************************************************************************
/// Setters
///*********************************************************************************************************************
void Hero::setDirection(glm::vec3 newDir){ _direction = newDir; }
void Hero::setRadius(GLfloat newRadius){ _radius = newRadius; }