#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <shader.hpp>
#include "stb_image.h"

// Automatically resizes the viewport when the window is resized
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

float vertices[] = {
    // positions        colors            texture coords
    0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
    0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
    -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top left
};

int width, height, nrChannels;
float texCoords[]{
    0.0f, 0.0f, // lower-left corner
    1.0f, 0.0f, // lower-right corner
    0.5f, 1.0f, // top-center corner
};

/**
 * 3----0
 * |  / |
 * | /  |
 * 2----1
 * Triangle 1 is indices 0-1-2, triangle 2 is indices 0-2-3
 */
// For use with EBO
unsigned int indices[] = {
    0, 1, 2, 0, 2, 3, // Two triangles of a rectangle
};

unsigned int VBO;
unsigned int VAO;
unsigned int EBO;
unsigned int texture1, texture2;

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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          (void *)(6 * sizeof(float)));
    // Argument is the vertex attribute position, in this case 0. Attributes are
    // disabled by default
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    // Flip images when loading them (otherwise everything is upside-down)
    stbi_set_flip_vertically_on_load(true);
    // Textures
    // Not sure if this needs to be inside the VAO declaration
    // Loads a JPG file and gives us data about it to use
    unsigned char *data =
        stbi_load("container.jpg", &width, &height, &nrChannels, 0);
    if (data) {
        // It seems the whole process for generating and binding objects is the
        // same across OpenGL, which is nice
        glGenTextures(1, &texture1);
        // VAO but for textures
        glActiveTexture(GL_TEXTURE0);
        // It is necessary that this is bound before parameters are set
        glBindTexture(GL_TEXTURE_2D, texture1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                        GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        /** Arguments are:
         *    Texture target: GL_TEXTURE_2D is a single texture target that I
         *    guess affects all texture objects bound to it? In our case the
         *    texture we just generated
         *
         *    Mipmap level: What the fuck is a mipmap (this argument allows for
         *    textures to be set for each mipmap level)
         *    Note: mipmaps are LOD for textures, basically. Would help if
         *    I hadn't skipped through the tutorial
         *
         *    Texture format: RGB, HSV, etc I would assume
         *
         *    Width and height of texture
         *
         *    Always 0 (legacy)
         *
         *    Format of source image (same possible values as texture format)
         *
         *    Datatype of source image
         *
         *    Actual image data */
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                     GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    } else {
        std::cout << "Failed to load texture image" << std::endl;
    }
    stbi_image_free(data);

    // Second texture
    data = stbi_load("awesomeface.png", &width, &height, &nrChannels, 0);

    if (data) {
        glGenTextures(1, &texture2);
        // VAO but for textures
        glActiveTexture(GL_TEXTURE1);
        // Texture must be bound for anything after this to take effect, which
        // does actually make sense
        glBindTexture(GL_TEXTURE_2D, texture2);
        // These don't actually have to be set again, because they were attached
        // to the GL_TEXTURE_2D object when we added the first texture
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
        //                 GL_LINEAR_MIPMAP_LINEAR);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Image is PNG so there is an alpha channel to account for
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture 2 image" << std::endl;
    }
    stbi_image_free(data);

    // Unbind the vertex array
    // Bindings done after this point are no longer linked to a VAO
    glBindVertexArray(0);

    myShader.use();
    myShader.setInt("texture1", 0);
    myShader.setInt("texture2", 1);
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
        // Looks like it may not be a part of the VAO after all
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        // Bind the VAO corresponding to the triangle we drew
        glBindVertexArray(VAO);
        // Bind 2D texture
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
