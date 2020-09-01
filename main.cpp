#include <iostream>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// Other includes
#include "shader.h"
#include "mesh.h"
#include "gui.h"
#include "camera.h"
#include "marchingcubes.h"

using namespace std;

const int width = 1200;
const int height = 800;

void setCameraDefaults(Mesh *mesh, Camera *camera);

int main() {
    // Init GLFW
	glfwInit();
	// Set all the required options for GLFW
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Create a GLFWwindow object
	GLFWwindow* window = glfwCreateWindow(width, height, "Assignment 2", nullptr, nullptr);
	if (window == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
    }
	glfwMakeContextCurrent(window);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Allow shader to change size of point
	glEnable(GL_PROGRAM_POINT_SIZE);

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    glViewport(0, 0, width, height);
    glfwSwapInterval(1);
    glfwSwapBuffers(window);

	// Create camera
	Camera* camera = new Camera();
	
	// Create gui
    GUI gui(window, camera);

    glewExperimental = GL_TRUE;
    glewInit();

    // Load shaders
    Shader shader("shader/basic.vert", "shader/basic.frag");

    // Mesh object
    /* Mesh *mesh = new Mesh(); */
	/* string meshPath = gui.modelName; */
	/* mesh->load(meshPath); */

	// Set initial camera position
	/* setCameraDefaults(mesh, camera); */

    glm::vec4 pointLightPosition;
    glm::vec4 pointLight2Position;
    pointLightPosition = glm::vec4(camera->position.x, camera->position.y, camera->position.z, 0.0f);
    pointLight2Position = glm::vec4(camera->position.x, camera->position.y, camera->position.z, 0.0f);

    MarchingCubes mc;
    Mesh mesh;

    GLfloat level = 0.0f;
    int cuts = 0;
    ModelName modelName;

    // Main loop
    while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		// Check if a new mesh path has been input
		/* if(meshPath != gui.modelName) { */
		/* 	meshPath = gui.modelName; */
		/* 	mesh->load(meshPath); */
		/* 	setCameraDefaults(mesh, camera); */
		/* 	gui.reset(); */
		/* } */
        bool update = false;
        if(modelName.name != gui.getModelType().name){
            modelName = gui.getModelType();
            update = true;
            mc.loadModel(modelName.name, modelName.x, modelName.y, modelName.z);
        }
        if(cuts != gui.cuts || update) {
            cuts = gui.cuts;
            update = true;
            mc.setCuts(cuts);
        }
        if(level != gui.depth || update) {
            level = gui.depth;
            vector<Face*> faces = mc.construct(level);
            mesh.createMesh(faces);
            mc.cleanUp();
        }

		// Render
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		// Culling
		glEnable(GL_DEPTH_TEST);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDepthFunc(gui.getDepthType());

		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);
		glFrontFace(GL_CW);


		// Update width and height
		glfwGetFramebufferSize(window, &width, &height);

		// Coordinate spaces
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = camera->getView();
		glm::mat4 projection = glm::perspective(glm::radians(gui.fov), (float)width / height, gui.zNear, gui.zFar);

        pointLightPosition = glm::rotateY(pointLightPosition, glm::radians(0.25f));
        pointLight2Position = glm::rotateX(pointLight2Position, glm::radians(0.25f));
		
		// Load and setup the shaders
        shader.use();
        shader.setMat4("model", model);
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        shader.setVec3("cameraPos", camera->position);
        shader.setVec3("colorIn", GUI::vec3(gui.color));
        shader.setFloat("shininess", gui.shininess);

        shader.setVec3("point.ambient", GUI::vec3(gui.point.ambient));
        shader.setVec3("point.diffuse", GUI::vec3(gui.point.diffuse));
        shader.setVec3("point.specular", GUI::vec3(gui.point.specular));
        shader.setVec4("point.position", pointLightPosition);
        shader.setBool("point.enabled", gui.point.status);
        shader.setVec3("point2.ambient", glm::vec3(0.0f));
        shader.setVec3("point2.diffuse", glm::vec3(0.2f, 0.0f, 0.3f));
        shader.setVec3("point2.specular", glm::vec3(0.0f, 0.6f, 0.2f));
        shader.setVec4("point2.position", pointLight2Position);
        shader.setBool("point2.enabled", gui.point.status);

        shader.setVec3("directional.ambient", GUI::vec3(gui.directional.ambient));
        shader.setVec3("directional.diffuse", GUI::vec3(gui.directional.diffuse));
        shader.setVec3("directional.specular", GUI::vec3(gui.directional.specular));
        glm::vec3 cameraLight = camera->u * gui.directionalX + camera->v * gui.directionalY + camera->n * gui.directionalZ * -1.0f;
        shader.setVec4("directional.position", glm::vec4(cameraLight.x, cameraLight.y, cameraLight.z, 1.0f));
        shader.setBool("directional.enabled", gui.directional.status);

        shader.setBool("shadeFlat", gui.getShadingType());

        shader.setInt("diffuseTexture", 0);
        shader.setInt("normalTexture", 1);

		// Draw the mesh
		glPolygonMode(GL_FRONT_AND_BACK, gui.getRenderType());
		mesh.draw();
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		
        // Draw the gui
        gui.draw();

		// Swap the screen buffers
		glfwSwapBuffers(window);
	}
}

