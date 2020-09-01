#ifndef GUI_H
#define GUI_H

#include <iostream>
#include <vector>

// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

#include <nanogui/nanogui.h>
#include "camera.h"

using namespace nanogui;

enum render_type {
    Point,
	Line,
	Triangle
};

enum culling_type {
    CW,
	CCW
};

enum shading_type {
    Smooth,
    Flat
};

enum depth_type {
    Less,
    Always
};

enum model_type {
    Bonsai,
    BostonTeapot,
    Bucky,
    Head
};

struct Light {
    bool status = true;
    Color ambient;
    Color diffuse;
    Color specular;
};

struct ModelName {
    std::string name;
    int x, y, z;
};

Screen *screen;

class GUI {
public:
    Color color;
    GLfloat zNear = 0.05f, zFar = 10.0f, fov = 45.0f;
    std::string modelName = "cube";
    GLfloat shininess = 20.0f;
    Light directional, point;
    bool pointRotateX, pointRotateY=true, pointRotateZ=false, resetPointLight = true;
    GLfloat directionalX = 0.0f, directionalY = -1.0f, directionalZ = -1.0f;
    bool showDiffuse, showNormal;
    GLfloat depth = 0.1f;
    int cuts = 100;

private:
    GLfloat rotateVal = 5.0f, moveVal = 0.4f;
    render_type renderType = Triangle;
    culling_type cullingType = CW;
    shading_type shadingType = Smooth;
    depth_type depthType = Less;
    model_type modelType = BostonTeapot;
    Camera* camera;

    nanogui::detail::FormWidget<GLfloat> *fovIn;
    nanogui::detail::FormWidget<bool> *pointRotateXIn, *pointRotateYIn, *pointRotateZIn;
    
public:
    GUI(GLFWwindow* window, Camera* camera) {
        this->camera = camera;
        // Set the default color
        color = Color(0.5f, 0.5f, 0.5f, 1.0f);
        directional.ambient = Color(0.2f, 0.2f, 0.5f, 1.0f);
        directional.diffuse = Color(0.8f, 0.8f, 0.8f, 1.0f);
        directional.specular = Color(0.8f, 0.8f, 0.8f, 1.0f);
        point.diffuse = Color(0.5f, 0.0f, 0.0f, 1.0f);
        point.specular = Color(1.0f, 0.5f, 0.0f, 1.0f);

        // Create a nanogui screen and pass the glfw pointer to initialize
        screen = new Screen();
        screen->initialize(window, true);

        // Create nanogui gui
        FormHelper *gui = new FormHelper(screen);

        // Camera controls
        gui->addWindow(Eigen::Vector2i(1200 - 175, 10), "Camera");

        gui->addGroup("Camera Position");
        gui->addVariable("Move value", moveVal)->setSpinnable(true);
        gui->addButton("Move left", [&]() { 
            moveCamera('u', -1.0f * moveVal);
        });
        gui->addButton("Move right", [this]() { 
            moveCamera('u', moveVal);
        });
        gui->addButton("Move up", [this]() { 
            moveCamera('v', -1.0f * moveVal);
        });
        gui->addButton("Move down", [this]() { 
            moveCamera('v', moveVal);
        });
        gui->addButton("Move forward", [this]() { 
            moveCamera('n', moveVal);
        });
        gui->addButton("Move backwards", [this]() { 
            moveCamera('n', -1.0f * moveVal);
        });

        gui->addGroup("Camera Rotation");
        gui->addVariable("Rotate value", rotateVal)->setSpinnable(true);
        gui->addButton("Rotate left", [&]() { 
            rotateCameraV(rotateVal);
        });
        gui->addButton("Rotate right", [this]() { 
            rotateCameraV(-1.0f * rotateVal);
        });
        gui->addButton("Rotate up", [this]() { 
            rotateCameraU(rotateVal);
        });
        gui->addButton("Rotate down", [this]() { 
            rotateCameraU(-1.0f * rotateVal);
        });
        gui->addButton("Rotate CW", [this]() { 
            rotateCameraN(rotateVal);
        });
        gui->addButton("Rotate CCW", [this]() { 
            rotateCameraN(-1.0f * rotateVal);
        });

        fovIn = gui->addVariable("FOV", fov);
        fovIn->setSpinnable(true);
        gui->addButton("Reset Front", [this]() { 
            reset();
        });
        gui->addButton("Reset Top", [this]() { 
            resetTop();
        });

        // General controls
        Window* frame = gui->addWindow(Eigen::Vector2i(10, 10), "General");

        gui->addGroup("Configuration");
        gui->addVariable("Render type", renderType)->setItems({ "Point", "Line", "Triangle" });
        gui->addVariable("Shading type", shadingType)->setItems({ "Smooth", "Flat" });
        gui->addVariable("Model name", modelType)->setItems({ "Bonsai", "BostonTeapot", "Bucky", "Head" });
        Slider* depthSlider = new Slider(frame);
        depthSlider->setValue(depth);
        gui->addWidget("View depth", depthSlider);
        depthSlider->setCallback([&](float value) {
            depth = value;
        });
        gui->addVariable("Cuts", cuts);
        
        // Lighting controls
        /* gui->addWindow(Eigen::Vector2i(10, 10), "Lighting"); */
        /* gui->addVariable("Object color", color); */
        /* gui->addVariable("Object shininess", shininess); */
        /* gui->addGroup("Directional Light"); */
        /* gui->addVariable("Status", directional.status); */
        /* gui->addVariable("Direction x", directionalX)->setSpinnable(true); */
        /* gui->addVariable("Direction y", directionalY)->setSpinnable(true); */
        /* gui->addVariable("Direction z", directionalZ)->setSpinnable(true); */
        /* gui->addVariable("Ambient color", directional.ambient); */
        /* gui->addVariable("Diffuse color", directional.diffuse); */
        /* gui->addVariable("Specular color", directional.specular); */

        /* gui->addGroup("Point Light"); */
        /* gui->addVariable("Status", point.status); */
        /* gui->addVariable("Ambient color", point.ambient); */
        /* gui->addVariable("Diffuse color", point.diffuse); */
        /* gui->addVariable("Specular color", point.specular); */
        /* pointRotateXIn = gui->addVariable("Rotate on X", pointRotateX); */
        /* pointRotateYIn = gui->addVariable("Rotate on Y", pointRotateY); */
        /* pointRotateZIn = gui->addVariable("Rotate on Z", pointRotateZ); */
        /* gui->addVariable("Diffuse status", showDiffuse); */
        /* gui->addVariable("Normal status", showNormal); */
        /* gui->addButton("Reset Point Light", [&]() { */ 
        /*     resetPointLight = true; */
        /* }); */


        // Render the widgets
        screen->setVisible(true);
        screen->performLayout();

        // Handle Events
        glfwSetCursorPosCallback(window,
            [](GLFWwindow *, double x, double y) {
                screen->cursorPosCallbackEvent(x, y);
            }
        );

        glfwSetMouseButtonCallback(window,
            [](GLFWwindow *, int button, int action, int modifiers) {
                screen->mouseButtonCallbackEvent(button, action, modifiers);
            }
        );

        glfwSetKeyCallback(window,
            [](GLFWwindow *, int key, int scancode, int action, int mods) {
                screen->keyCallbackEvent(key, scancode, action, mods);
            }
        );

        glfwSetCharCallback(window,
            [](GLFWwindow *, unsigned int codepoint) {
                screen->charCallbackEvent(codepoint);
            }
        );

        glfwSetDropCallback(window,
            [](GLFWwindow *, int count, const char **filenames) {
                screen->dropCallbackEvent(count, filenames);
            }
        );

        glfwSetScrollCallback(window,
            [](GLFWwindow *, double x, double y) {
                screen->scrollCallbackEvent(x, y);
            }
        );

        glfwSetFramebufferSizeCallback(window,
            [](GLFWwindow *, int width, int height) {
                screen->resizeCallbackEvent(width, height);
                glViewport(0, 0, width, height);
            }
        );
    }

    void draw() {
        screen->drawWidgets();
    }

    void reset() {
        fov = 45.0f;
        fovIn->setValue(fov);
        camera->resetCameraFront();
    }

    void resetTop() {
        camera->resetCameraTop();
    }

    void moveCamera(char direction, GLfloat units) {
        switch(direction) {
            case 'n':
                camera->moveN(units);
                break;
            case 'u':
                camera->moveU(units);
                break;
            case 'v':
                camera->moveV(units);
                break;
        }
    }

    ModelName getModelType() {
        switch(modelType) {
            case BostonTeapot:
                return {
                    "models/BostonTeapot_256_256_178.raw", 256, 256, 178
                };
            case Bonsai:
                return {
                    "models/Bonsai_512_512_154.raw", 512, 512, 154
                };
            case Bucky:
                return {
                    "models/Bucky_32_32_32.raw", 32, 32, 32
                };
            case Head:
                return {
                    "models/Head_256_256_225.raw", 256, 256, 225
                };
        }
    }

    void rotateCameraU(GLfloat degrees) {
        camera->rotateU(degrees);
    }

    void rotateCameraV(GLfloat degrees) {
        camera->rotateV(degrees);
    }

    void rotateCameraN(GLfloat degrees) {
        camera->rotateN(degrees);
    }

    GLenum getRenderType() {
        switch(renderType) {
            case Point:
                return GL_POINT;
            case Line:
                return GL_LINE;
            case Triangle:
                return GL_FILL;
        }
    }

    GLenum getCullingType() {
        switch(cullingType) {
            case CW:
                return GL_CW;
            case CCW:
                return GL_CCW;
        }
    }

    bool getShadingType() {
        switch(shadingType) {
            case Smooth:
                return false;
            case Flat:
                return true;
        }
    }

    GLenum getDepthType() {
        switch(depthType) {
            case Less:
                return GL_LESS;
            case Always:
                return GL_ALWAYS;
        }
    }

    static glm::vec3 vec3(const Color &color) {
        return glm::vec3(color.r(), color.g(), color.b());
    }
};

#endif
