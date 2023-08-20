#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>

// Automatically resizes the viewport when the window is resized
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

float vertices[] = {-0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f};

unsigned int VBO;
unsigned int VAO;
const char *vertexShaderSource =
    "#version 460 compatibility \n"
    "layout (location = 0) in vec3 aPos; \n"
    "void main(){\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0); \n"
    "}\0";
unsigned int vertexShader;

const char *fragShaderSource = "#version 460 compatibility\n"
                               "out vec4 FragColor;\n"
                               "void main() {\n"
                               "FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                               "}\n";
unsigned int fragShader;

unsigned int shaderProgram;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    // Create the GL window
    GLFWwindow *window = glfwCreateWindow(800, 600, "Ray Tracer", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to init GL window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Load GLAD bindings to use OpenGL functions
    if (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)) {
        std::cout << "Failed to init GLAD" << std::endl;
        return -1;
    }
    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Create and compile the vertex shader
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Make sure vertex shader compilation succeeded
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR: Vertex Shader Compilation failed\n"
                  << infoLog << std::endl;
    }

    // Create and compile the fragment shader
    fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragShaderSource, NULL);
    glCompileShader(fragShader);

    // Create a shader program (linked version of all shaders) and attach the
    // vertex and frag shaders to it
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragShader);
    glLinkProgram(shaderProgram);

    // Create a Vertex Array Object (VAO) to contain all of the Vertex Attribute
    // associations
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    // Generate vertex buffer objects (VBOs) to pass the GPU large amounts of
    // data quickly
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glVertexAttribPointer.xhtml
    // Also https://learnopengl.com/Getting-started/Hello-Triangle around 1/3rd
    // of the way down
    // This is bound to the vertices array due to the previous (and only) VBO
    // being bound to the array buffer when we called this.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void *)0);
    // Argument is the vertex attribute position, in this case 0. Attributes are
    // disabled by default
    glEnableVertexAttribArray(0);

    // Unbind the vertex array
    glBindVertexArray(0);

    // Use the shader program and delete the shader objects
    glUseProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragShader);
    // Render loop
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        glClearColor(0.2f, 0.5f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
