#include <shader.hpp>

Shader::Shader(const char *vertexPath, const char *fragmentPath) {
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    // Exceptions? Need to research this
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;

        // I will not pretend to understand the mechanisms of streams. I know
        // this reads the files, but does it read the whole file at once?
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        // Have to assume so, but then why a stream? So we can read it bit by
        // bit later?
        vShaderFile.close();
        fShaderFile.close();

        // Maybe this is just a convoluted C++ version of fread()
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    } catch (std::ifstream::failure e) {
        std::cout << "Error: Shader files not successfully read" << std::endl;
    }
    // This is just the C++ equivalent of fread()???
    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();

    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    // Basically repeat the process to build the shaders that was done manually
    // earlier
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);

    // Make sure vertex shader compilation succeeded
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ERROR: Vertex Shader Compilation failed\n"
                  << infoLog << std::endl;
    }

    // Create and compile the fragment shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);

    // Make sure fragment shader compilation succeeded
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        std::cout << "ERROR: Fragment Shader Compilation failed\n"
                  << infoLog << std::endl;
    }

    // Create a shader program (linked version of all shaders) and attach the
    // vertex and frag shaders to it
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);

    // std::cout << "Here" << std::endl;
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "ERROR: Shader program linking failed\n"
                  << infoLog << std::endl;
    }
    // std::cout << "Here" << std::endl;
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use() { glUseProgram(ID); }

void Shader::setBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
