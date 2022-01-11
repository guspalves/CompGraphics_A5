//
// Created by Gus Alves on 10/4/21.
//

#ifndef A3_ARCBALL_H
#define A3_ARCBALL_H

class ArcBall{
public:
    /// \desc function to recalculate position of camera
    void recomputeOrientation(GLfloat camDistance);

    /// \desc function that calculates the view matrix for the arcball camera
    void computeViewMatrix(GLfloat xOffset, GLfloat zOffset);

    /// \desc setters
    void setPosition(glm::vec3 position);
    void setTheta(GLfloat theta);
    void setPhi(GLfloat phi);

    /// \desc getters
    GLfloat getTheta();
    GLfloat getPhi();
    glm::mat4 getViewMatrix();

private:
    /// \desc needed variables for camera implementation
    glm::vec3 _position;
    GLfloat _theta;
    GLfloat _phi;
    glm::mat4 _viewMatrix;
};

/// ****************************************************************************************************************
/// function implementations
/// ****************************************************************************************************************

inline void ArcBall::setPosition(glm::vec3 position){
    _position = position;
}

inline void ArcBall::setTheta(GLfloat theta){
    _theta = theta;
}

inline void ArcBall::setPhi(GLfloat phi){
    _phi = phi;
}

inline glm::mat4 ArcBall::getViewMatrix(){
    return _viewMatrix;
}

inline GLfloat ArcBall::getTheta(){
    return _theta;
}

inline GLfloat ArcBall::getPhi(){
    return _phi;
}

inline void ArcBall::recomputeOrientation(GLfloat camDistance){
    _position.x = camDistance * sin(_theta) * sin(_phi);
    _position.y = -camDistance * cos(_phi);
    _position.z = -camDistance * cos(_theta) * sin(_phi);
}

inline void ArcBall::computeViewMatrix(GLfloat xOffset, GLfloat zOffset){
    _viewMatrix = glm::lookAt(_position + glm::vec3(2.25+xOffset, 0, -2.5+zOffset), glm::vec3(xOffset, 0.0, zOffset), glm::vec3(0, 1, 0));
}

#endif //A3_ARCBALL_H
