#include "Camera.h"

float DTR = M_PI/180.0f;
float RTD = 180.0f/M_PI;

void Camera::rotate(float degrees) {

    //Logger::Log("degrees = " + std::to_string(degrees));
    float radials = degrees * DTR;
    //Logger::Log("radial = " + std::to_string(radials/M_PI));
    angle += radials;
    //Logger::Log("angle = " + std::to_string(angle/M_PI));

}
