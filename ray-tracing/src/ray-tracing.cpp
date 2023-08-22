#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <shader.hpp>

// Automatically resizes the viewport when the window is resized
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

float vertices[] = {
    0.0f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // 1 bottom right
    -1.0f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // 1 bottom left
    -0.5f, 0.5f,  0.0f, 0.0f, 0.0f, 1.0f, // 1 top
    1.0f,  -0.5f, 0.0f, 0.0f, 0.5f, 1.0f, // 2 bottom right
    0.0f,  -0.5f, 0.0f, 0.5f, 0.0f, 1.0f, // 2 bottom left
    0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.5f, // 2 top
};

// For use with EBO
unsigned int indices[] = {
    0, 1, 2, // first triangle
    3, 4, 5  // second triangle
};

unsigned int VBO;
unsigned int VAO;
unsigned int EBO;

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

    // Create a Vertex Array Object (VAO) to contain all of the Vertex Attribute
    // associations.
    // All bindings done after this point are linked to this VAO
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    Shader myShader("./vert.glsl", "./frag.glsl");
    // myShader.use();
    //  Generate Vertex Buffer Object (VBO) to pass the GPU large amounts of
    //  data quickly
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Generate and bind Element Buffer Object (EBO) to allow OpenGL to draw
    // vertices from a set of unique vertices and indices
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                 GL_STATIC_DRAW);

    // https://registry.khronos.org/OpenGL-Refpages/gl4/html/glVertexAttribPointer.xhtml
    // Also https://learnopengl.com/Getting-started/Hello-Triangle around 1/3rd
    // of the way down
    // This is bound to the vertices array due to the previous (and only) VBO
    // being bound to the array buffer when we called this.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void *)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    // Argument is the vertex attribute position, in this case 0. Attributes are
    // disabled by default
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Unbind the vertex array
    // Bindings done after this point are no longer linked to a VAO
    glBindVertexArray(0);

    // Render loop
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        // Clear the screen
        glClearColor(0.2f, 0.5f, 0.9f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // float timeValue = glfwGetTime();
        // float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
        // int vertexColorLocation = glGetUniformLocation(myShader.ID,
        // "ourColor"); glUniform4f(vertexColorLocation, 0.0f, greenValue,
        // 0.0f, 1.0f);

        myShader.use();
        // Bind the VAO corresponding to the triangle we drew
        glBindVertexArray(VAO);
        // Draw the shape
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // Unbind the VAO
        glBindVertexArray(0);
        // Display the drawn image to the screen all at once
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
