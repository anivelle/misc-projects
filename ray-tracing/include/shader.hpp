// This shader class is taken from the Learn OpenGL page on shaders. I feel it
// will be useful for future shaders
#ifndef SHADER_H
#define SHADER_H

#include <glad/gl.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
  public:
    unsigned int ID;
    Shader(const char *vertexPath, const char *fragmentPath);
    void use();
    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
};
#endif