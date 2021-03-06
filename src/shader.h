/**************************************
 * shader.h
 * sourced from opengl tutorial website:
 * http://learnopengl.com/
 */

#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
    GLuint Program;
    // Constructor generates the shader on the fly
    Shader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath = nullptr) {
        // 1. Retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::string geometryCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        std::ifstream gShaderFile;
        // ensures ifstream objects can throw exceptions:
        vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        gShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        try {
            // Open files
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // Read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();        
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // Convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();            
            // If geometry shader path is present, also load a geometry shader
            if(geometryPath != nullptr) {
                gShaderFile.open(geometryPath);
                std::stringstream gShaderStream;
                gShaderStream << gShaderFile.rdbuf();
                gShaderFile.close();
                geometryCode = gShaderStream.str();
            }
        }
        catch (std::ifstream::failure e) {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }
        const GLchar* vShaderCode = vertexCode.c_str();
        const GLchar * fShaderCode = fragmentCode.c_str();
        // 2. Compile shaders
        GLuint vertex, fragment;
        // Vertex Shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX");
        // Fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT");
        // If geometry shader is given, compile geometry shader
        GLuint geometry;
        if(geometryPath != nullptr) {
            const GLchar * gShaderCode = geometryCode.c_str();
            geometry = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometry, 1, &gShaderCode, NULL);
            glCompileShader(geometry);
            checkCompileErrors(geometry, "GEOMETRY");
        }
        // Shader Program
        this->Program = glCreateProgram();
        glAttachShader(this->Program, vertex);
        glAttachShader(this->Program, fragment);
        if(geometryPath != nullptr)
            glAttachShader(this->Program, geometry);
        glLinkProgram(this->Program);
        checkCompileErrors(this->Program, "PROGRAM");
        // Delete the shaders as they're linked into our program now 
        // and are no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        if(geometryPath != nullptr)
            glDeleteShader(geometry);

    }
    // Uses the current shader
    void use() { glUseProgram(this->Program); }

    // Some helper utitlities
    void setBool(const std::string &name, bool value) const {         
        glUniform1i(glGetUniformLocation(this->Program, name.c_str()), (int)value); 
    }
    void setInt(const std::string &name, int value) const { 
        glUniform1i(glGetUniformLocation(this->Program, name.c_str()), value); 
    }
    void setFloat(const std::string &name, float value) const { 
        glUniform1f(glGetUniformLocation(this->Program, name.c_str()), value); 
    } 
    void set2Float(const std::string &name, float v1, float v2) {
        GLint id = glGetUniformLocation(this->Program, name.c_str());
        glUniform2f(id, v1, v2);
    }
    void set3Float(const std::string &name, float v1, float v2, float v3) {
        GLint id = glGetUniformLocation(this->Program, name.c_str());
        glUniform3f(id, v1, v2, v3);
    }
    void set3Float(const std::string &name, glm::vec3 v) {
        GLint id = glGetUniformLocation(this->Program, name.c_str());
        glUniform3f(id, v.x, v.y, v.z);
    }
    void set4Float(const std::string &name, 
                   float v1, float v2, float v3, float v4) {
        GLint id = glGetUniformLocation(this->Program, name.c_str());
        glUniform4f(id, v1, v2, v3, v4);
    }

    void setMat4(const std::string& name, glm::mat4 mat4) {
      GLint id = glGetUniformLocation(this->Program, name.c_str());
      glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(mat4));
    }

private:
    void checkCompileErrors(GLuint shader, std::string type) {
        GLint success;
        GLchar infoLog[1024];
        if(type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if(!success) {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "| ERROR::::SHADER-COMPILATION-ERROR of type: " << type << "|\n" << infoLog << "\n| -- --------------------------------------------------- -- |" << std::endl;
            }
        }
        else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if(!success) {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "| ERROR::::PROGRAM-LINKING-ERROR of type: " << type << "|\n" << infoLog << "\n| -- --------------------------------------------------- -- |" << std::endl;
            }
        }
    }
};

#endif
