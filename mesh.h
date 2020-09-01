#ifndef MESH_H
#define MESH_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <regex>

// GLEW
#include <GL/glew.h>

// glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "marchingcubes.h"

using namespace std;

/* struct Tangent { */
/*     GLfloat tx, ty, tz; */
/*     GLfloat bx, by, bz; */
/* }; */

struct Vertex {
    glm::vec3 p;
    glm::vec3 n;
};

class Mesh {
public:
    size_t size;
    GLuint VAO;
    GLuint VBO;

private:
    bool loaded = false;

public:
    void createMesh(vector<Face*> &faces) {
        const GLuint stride = 6;

        vector<Vertex> vertices;
        for(size_t i = 0; i < faces.size(); i++) {
            /* cout<<points[i].x<<" "<<points[i].y<<" " <<points[i].z<<endl; */
            for(size_t j = 0; j < 3; j++) {
                vertices.push_back({
                    faces[i]->iList[j]->position,
                    faces[i]->iList[j]->normal
                });
            }
        }
        // Clean up
        /* for(size_t i = 0; i < faces.size(); i++) { */
        /*     for(size_t j = 0; j < 3; j++) { */
        /*         delete faces[i]->iList[j]; */
        /*     } */
        /*     delete faces[i]; */
        /* } */

        size = vertices.size();
        cout<<size/3<< " triangles" <<endl;

        if(!loaded) {
            loaded = true;

            // create vertex array object
            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);

            // create vertex buffer object
            glGenBuffers(1, &VBO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * stride * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);

            // Positions
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (void*)(0 * sizeof(GLfloat)));
            glEnableVertexAttribArray(0); 
            // Normals
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
            glEnableVertexAttribArray(1); 
        } else {
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * stride * sizeof(GLfloat), &vertices[0], GL_STATIC_DRAW);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    // Draw the currently loaded mesh
    void draw() {
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, size);
        glBindVertexArray(0);
    }
};

#endif
