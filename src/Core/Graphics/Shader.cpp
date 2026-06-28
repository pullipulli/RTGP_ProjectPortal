//
// Created by Andrea Pullia on 28/06/2026.
//

#include "Core/Graphics/Shader.h"

const std::string& Shader::GetShaderId() const
{
    return shaderId;
}

Shader::Shader(const std::string& shaderId, const GLchar* vertexPath, const GLchar* fragmentPath)
{
    // Step 1: we retrieve shaders source code from provided filepaths
    string vertexCode;
    string fragmentCode;
    ifstream vShaderFile;
    ifstream fShaderFile;

    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions (ifstream::failbit | ifstream::badbit);
    fShaderFile.exceptions (ifstream::failbit | ifstream::badbit);
    try
    {
        // Open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        stringstream vShaderStream, fShaderStream;
        // Read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // Convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (ifstream::failure const&)
    {
        cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << endl;
    }

    // Convert strings to char pointers
    const GLchar* vShaderCode = vertexCode.c_str();
    const GLchar * fShaderCode = fragmentCode.c_str();

    // Step 2: we compile the shaders
    GLuint vertex, fragment;

    // Vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, nullptr);
    glCompileShader(vertex);
    // check compilation errors
    checkCompileErrors(vertex, "VERTEX");

    // Fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, nullptr);
    glCompileShader(fragment);
    // check compilation errors
    checkCompileErrors(fragment, "FRAGMENT");

    // Step 3: Shader Program creation
    this->Program = glCreateProgram();
    glAttachShader(this->Program, vertex);
    glAttachShader(this->Program, fragment);
    glLinkProgram(this->Program);
    // check linking errors
    checkCompileErrors(this->Program, "PROGRAM");

	this->shaderId = shaderId;

    // Step 4: we delete the shaders because they are linked to the Shader Program, and we do not need them anymore
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::Use() { glUseProgram(this->Program); }

void Shader::Delete() { glDeleteProgram(this->Program); }

void Shader::checkCompileErrors(GLuint shader, string type)
{
    GLint success;
    GLchar infoLog[1024];
    if(type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            cout << "| ERROR::::SHADER-COMPILATION-ERROR of type: " << type << "|\n" << infoLog << "\n| -- --------------------------------------------------- -- |" << endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if(!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            cout << "| ERROR::::PROGRAM-LINKING-ERROR of type: " << type << "|\n" << infoLog << "\n| -- --------------------------------------------------- -- |" << endl;
        }
    }
}

void Shader::SetUniformParameter(const std::string& parameterName, float value)
{
    GLint uniformLocation = glGetUniformLocation(Program, parameterName.c_str());

    glUniform1f(uniformLocation, value);
}

void Shader::SetUniformParameter(const std::string& parameterName, int value)
{
    GLint uniformLocation = glGetUniformLocation(Program, parameterName.c_str());

    glUniform1i(uniformLocation, value);
}

void Shader::SetUniformParameter(const std::string& parameterName, bool value)
{
    GLint uniformLocation = glGetUniformLocation(Program, parameterName.c_str());

    glUniform1i(uniformLocation, value);
}

void Shader::SetUniformParameter(const std::string& parameterName, const glm::vec2* value)
{
    GLint uniformLocation = glGetUniformLocation(Program, parameterName.c_str());

    glUniform2fv(uniformLocation, 1, glm::value_ptr(*value));
}

void Shader::SetUniformParameter(const std::string& parameterName, const glm::vec3* value)
{
    GLint uniformLocation = glGetUniformLocation(Program, parameterName.c_str());

    glUniform3fv(uniformLocation, 1, glm::value_ptr(*value));
}

void Shader::SetUniformParameter(const std::string& parameterName, const glm::vec4* value)
{
    GLint uniformLocation = glGetUniformLocation(Program, parameterName.c_str());

    glUniform4fv(uniformLocation, 1, glm::value_ptr(*value));
}

void Shader::SetUniformParameter(const std::string& parameterName, const glm::mat2* value)
{
    GLint uniformLocation = glGetUniformLocation(Program, parameterName.c_str());

    glUniformMatrix2fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(*value));
}

void Shader::SetUniformParameter(const std::string& parameterName, const glm::mat3* value)
{
    GLint uniformLocation = glGetUniformLocation(Program, parameterName.c_str());

    glUniformMatrix3fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(*value));
}

void Shader::SetUniformParameter(const std::string& parameterName, const glm::mat4* value)
{
    GLint uniformLocation = glGetUniformLocation(Program, parameterName.c_str());

    glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(*value));
}