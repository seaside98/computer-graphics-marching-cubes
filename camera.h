#ifndef CAMERA_H
#define CAMERA_H

// GLEW
#include <GL/glew.h>

// glm
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <vector>

#include <iostream>

/**
 * Functions for moving and rotating the camera
 */
class Camera {
public:
    glm::vec3 u, v, n;
    glm::vec3 position;

    Camera() {
        resetCameraFront();
    }

    // Reset the camera to the default position
    void resetCameraFront() {
        u = glm::vec3(-1.0f, 0.0f, 0.0f);
        v = glm::vec3(0.0f, -1.0f, 0.0f);
        n = glm::vec3(0.0f, 0.0f, -1.0f);
        position = glm::vec3(0.5f, 0.5f, 2.5f);
    }

    // Reset the camera to the default position
    void resetCameraTop() {
        u = glm::vec3(-1.0f, 0.0f, 0.0f);
        v = glm::vec3(0.0f, 0.0f, -1.0f);
        n = glm::vec3(0.0f, 1.0f, 0.0f);
        position = glm::vec3(0.5f, -1.5f, 0.5f);
    }

    // Move the camera along its axis
    void moveU(const GLfloat units) {
        position += units * u;
    }

    void moveV(const GLfloat units) {
        position += units * v * glm::vec3(-1.0f);
    }

    void moveN(const GLfloat units) {
        position += units * n;
    }

    // To rotate, rotate the other 2 vectors around the selected one
    void rotateU(const GLfloat degrees) {
        v = glm::rotate(v, glm::radians(degrees), u);
        n = glm::rotate(n, glm::radians(degrees), u);
    }

    void rotateV(const GLfloat degrees) {
        u = glm::rotate(u, glm::radians(degrees), v);
        n = glm::rotate(n, glm::radians(degrees), v);
    }

    void rotateN(const GLfloat degrees) {
        v = glm::rotate(v, glm::radians(degrees), n);
        u = glm::rotate(u, glm::radians(degrees), n);
    }

    // Get the current view
    glm::mat4 getView() {
        return glm::lookAt(position, position + n, v);
    }
};

#endif
