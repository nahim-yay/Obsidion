

#pragma once



#include <gl/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include <obs/Player.h>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float ZOOM        =  45.0f;


// A Third Perspepctive Camera
class TPCamera
{
public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    
    // euler Angles
    float Yaw;
    float Pitch;
    float Roll;

    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    Player player;



    float distanceFromPlayer = 50;
    float angleAroundPlayer = 0;

    float horizontalDistance, verticalDistance;

    // constructor with vectors
    TPCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
    // constructor with scalar values
    TPCamera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    TPCamera(Player player, float horizontalDistance, float verticalDistance)
    {
        this->player = player;
        this->horizontalDistance = horizontalDistance;
        this->verticalDistance   = verticalDistance;
        // this->angleAroundPlayer  = angleAroundPlayer;

        Position = glm::vec3
        (
            0,
            player.getPosition().y + verticalDistance,
            player.getPosition().z + horizontalDistance
        );

        Yaw = -90.0f;
        Pitch = 0.0f;

        WorldUp = glm::vec3(0,1,0);
        updateCameraVectors();
    }

    // void calculateCameraPosition()
    // {
    //     float theta = player.getOrientation().y + angleAroundPlayer;
    //     float offsetX = horizontalDistance * sin(glm::radians(theta));
    //     float offsetZ = horizontalDistance * cos(glm::radians(theta));

    //     Position.x = player.getPosition().x - offsetX;
    //     Position.z = player.getPosition().z - offsetZ;
    //     Position.y = player.getPosition().y + verticalDistance;
    // }

    void update()
    {
        // calculateCameraPosition();
        updateCameraVectors();
    }








    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, player.getPosition(), Up);
    }

























private:
    
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        glm::vec3 front;

        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        
        Front = glm::normalize(front);

        // also re-calculate the Right and Up vector
        // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up    = glm::normalize(glm::cross(Right, Front));
    }
};
