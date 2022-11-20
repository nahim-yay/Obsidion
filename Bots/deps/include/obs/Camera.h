

#pragma once

#include <iostream>

#include <gl/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include <obs/Object.h>




class Camera
{
public:
    
    glm::vec3 position;



    Camera()
    {}

    Camera(Object *object, float horizontalDistance, float verticalDistance)
    {
        this->object = object;
        this->horizontalDistance = horizontalDistance;
        this->verticalDistance = verticalDistance;

        position = glm::vec3(0, verticalDistance, horizontalDistance);

        init();
        

        updateCameraVectors();
    }


    void update(float uTime)
    {
        float theta = object->getOrientation().y;
        float offetX = horizontalDistance * sin(glm::radians(theta));
        float offetZ = horizontalDistance * cos(glm::radians(theta));

        position.x = object->getPosition().x + offetX;
        position.z = object->getPosition().z + offetZ;
        position.y = object->getPosition().y + verticalDistance;

        yaw = -90 - object->getOrientation().y;

        updateCameraVectors();
    }


    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(position, position + camFront, camUp);
    }


private:
    
    glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

    // camera Attributes
    glm::vec3 camFront;
    glm::vec3 camUp;
    glm::vec3 camRight;

    glm::vec3 camTarget;
    glm::vec3 camDirection;

    float distance, horizontalDistance, verticalDistance;
    
    float pitch, yaw, roll;

    Object *object;



    void init()
    {
        pitch = atan(verticalDistance / horizontalDistance);
        yaw = -90;
        distance = glm::sqrt(glm::pow(verticalDistance, 2) + glm::pow(horizontalDistance, 2));
    }


    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        glm::vec3 front;

        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

        camFront = glm::normalize(front);
 
        camFront = glm::normalize(object->getPosition() - position);
        std::cout << object->getPosition().x << " " << object->getPosition().y << " " << object->getPosition().z << "\n";


        // also re-calculate the Right and Up vector
        // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        camRight = glm::normalize(glm::cross(camFront, worldUp));
        camUp    = glm::normalize(glm::cross(camRight, camFront));
    }
};

