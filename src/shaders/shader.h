#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h> // include glad to get all the required OpenGL headers

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/matrix_float4x4.hpp>

namespace VoxelEngine {
    class Shader
    {
    public:
        // the program ID
        unsigned int ID;
        unsigned int viewLoc;
        unsigned int projectionLoc;
        unsigned int modelLoc;

        // constructor reads and builds the shader
        Shader(const char* vertexPath, const char* fragmentPath);

        // use/activate the shader
        void use();

        // utility uniform functions
        void setBool(const std::string& name, bool value) const;
        void setInt(const std::string& name, int value) const;
        void setFloat(const std::string& name, float value) const;

        void setVec3(const std::string& name, const glm::vec3& value) const;
        void setMat4(const std::string& name, const glm::mat4& value) const;

    };
}

#endif