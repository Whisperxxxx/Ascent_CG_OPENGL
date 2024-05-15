#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct SCamera {
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float Yaw;
    float Pitch;
    const float MovementSpeed = 0.1f;
    const float MouseSensitivity = 0.1f;

    void InitCamera(const glm::vec3& position, const glm::vec3& up, float yaw, float pitch) {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;

        updateCameraVectors();
    }

    void ProcessMouseMovement(float xoffset, float yoffset) {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;

        updateCameraVectors();
    }

    void updateCameraVectors() {
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch)); // The X-axis component of the camera on the XZ plane
        front.y = sin(glm::radians(Pitch)); // looking up or down
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch)); // The Z-axis component of the camera on the XZ plane


        Front = glm::normalize(front);
        Right = glm::normalize(glm::cross(Front, WorldUp));
        Up = glm::normalize(glm::cross(Right, Front));
    }

};
