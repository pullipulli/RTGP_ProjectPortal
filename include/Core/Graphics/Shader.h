/*
Shader class
- loading Shader source code, Shader Program creation

N.B. ) adaptation of https://github.com/JoeyDeVries/LearnOpenGL/blob/master/includes/learnopengl/shader.h

author: Davide Gadia

Real-Time Graphics Programming - a.a. 2024/2025
Master degree in Computer Science
Universita' degli Studi di Milano
*/

#pragma once

// Std. Includes
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

using namespace std;


/////////////////// SHADER class ///////////////////////
class Shader
{
public:

    GLuint Program;

	const std::string& GetShaderId() const;

    //////////////////////////////////////////

    //constructor
    Shader(const std::string& shaderId, const GLchar* vertexPath, const GLchar* fragmentPath);

    //////////////////////////////////////////

    // We activate the Shader Program as part of the current rendering process
    void Use();

    // We delete the Shader Program when application closes
    void Delete();

	void SetUniformParameter(const std::string& parameterName, float value);
	void SetUniformParameter(const std::string& parameterName, int value);
	void SetUniformParameter(const std::string& parameterName, bool value);
	void SetUniformParameter(const std::string& parameterName, const glm::vec2* value);
	void SetUniformParameter(const std::string& parameterName, const glm::vec3* value);
	void SetUniformParameter(const std::string& parameterName, const glm::vec4* value);
	void SetUniformParameter(const std::string& parameterName, const glm::mat2* value);
	void SetUniformParameter(const std::string& parameterName, const glm::mat3* value);
	void SetUniformParameter(const std::string& parameterName, const glm::mat4* value);

private:
    //////////////////////////////////////////

	std::string shaderId;

    // Check compilation and linking errors
    void checkCompileErrors(GLuint shader, string type);
};
