#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

// GLEW
#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;

class Shader {
public:
    GLuint program;

    // read and build the shader
    Shader(const char* vertexPath, const char* fragmentPath) {
        // read files
        std::string vertexCode, fragmentCode;
        std::ifstream vShaderFile, fShaderFile;
        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try {
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;

            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();

            vShaderFile.close();
            fShaderFile.close();

            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        } catch(std::ifstream::failure e) {
            std::cout << "Error: Failed to read shader file." << std::endl;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();

        // vertex shader
        GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        compile_shader(vertex, "Vertex");

        // fragement shader
        GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        compile_shader(fragment, "Fragment");

        // shader program
        program = glCreateProgram();
        glAttachShader(program, vertex);
        glAttachShader(program, fragment);
        glLinkProgram(program);
        compile_shader(program, "Program");

        // clean up shaders
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    }

    // activate the shader
    void use() {
        glUseProgram(program);
    }

    void setVec3(const GLchar *name, const glm::vec3 &vec) {
		glUniform3fv(glGetUniformLocation(program, name), 1, glm::value_ptr(vec));
    }

    void setVec4(const GLchar *name, const glm::vec4 &vec) {
		glUniform4fv(glGetUniformLocation(program, name), 1, glm::value_ptr(vec));
    }

    void setMat4(const GLchar *name, const glm::mat4 &mat) {
		glUniformMatrix4fv(glGetUniformLocation(program, name), 1, GL_FALSE, glm::value_ptr(mat));
    }

    void setBool(const GLchar *name, const bool var) {
		glUniform1i(glGetUniformLocation(program, name), var);
    }

    void setInt(const GLchar *name, const GLint var) {
		glUniform1i(glGetUniformLocation(program, name), var);
    }

    void setFloat(const GLchar *name, const GLfloat var) {
		glUniform1f(glGetUniformLocation(program, name), var);
    }

private:
    // Compile a shader or program
    void compile_shader(GLuint shader, string type) {
        GLint success = false;
        GLchar infoLog[512];

        // Handle fragment and vertex shaders
        if(type == "Fragment" || type == "Vertex") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if(!success) {
                glGetShaderInfoLog(shader, 512, NULL, infoLog);
            }

        // Handle program
        } else if(type == "Program") {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if(!success) {
                glGetProgramInfoLog(shader, 512, NULL, infoLog);
            }
        }

        // Print error
        if(!success) {
            std::cout << "Error: Shader " << type << " failed to compile.\n" << infoLog << std::endl;
        }
    }

};

#endif
