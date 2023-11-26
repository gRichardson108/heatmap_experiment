#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <malloc.h>
#include "rendering.h"
#include "shader.h"
#include <iostream>
#include <cstring>
#include "socketdatareceiver.h"


int screenWidth = 640;
int screenHeight = 480;

struct CircularTexture {
    /// image that holds the pixels backing the circular texture
    Image image;
    /// The texture object for OpenGL
    Texture2D texture;
    /// the starting pixel row to render
    int start_row;
    /// the last pixel row to render
    int end_row;
    /// the pixel row to write new data into
    int insert_row;
};
CircularTexture noiseImage;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

int main(void) {
    GLFWwindow *window;
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(screenWidth, screenHeight, "Noise renderer", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }


    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // setup GLAD
    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0) {
        fprintf(stderr, "Failed to initialize OpenGL context\n");
        return 1;
    }

    glClearColor(0.4f, 0.3f, 0.4f, 0.0f);
    CircularTexture noiseImage = {
            .image = {
                .data = malloc(sizeof(ColorRGBA) * 1920 * 1080),
                .width = screenWidth,
                .height = screenHeight
            },
            .start_row = 0,
            .end_row = screenHeight,
            .insert_row = 0
    };
    fillImageWithNoise(noiseImage.image, 0.5f);

    noiseImage.texture = allocateTexture(noiseImage.image);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
            // positions          // colors           // texture coords
            1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, // top right
            1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, // bottom right
            -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, // bottom left
            -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f  // top left
    };
    unsigned int indices[] = {
            0, 1, 3, // first triangle
            1, 2, 3  // second triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    Shader ourShader("resources/texture.vsh", "resources/texture.fsh");
    ourShader.use();
    glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0);
    auto textureOffsetId = glGetUniformLocation(ourShader.ID, "textureOffset");
    auto textureScaleId = glGetUniformLocation(ourShader.ID, "textureScale");

    double previousTime = glfwGetTime();
    int frameCount = 0;
    /* Loop until the user closes the window */
    GLfloat offset[2] = {};
    GLfloat scale[2] = {1.0f, 1.0f};
    glUniform2fv(textureOffsetId, 1, offset);
    glUniform2fv(textureScaleId, 1, scale);
    bool paused = true;
    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        frameCount++;
        if (currentTime - previousTime >= 1.0) {
            std::cout << frameCount << "fps\n";
            previousTime = currentTime;
            frameCount = 0;
        }

        // generate a random row of data
        ColorRGBA *pixels = (ColorRGBA *) noiseImage.image.data;
        for (int i = 0; i < screenWidth; i++) {
            pixels[i + (noiseImage.insert_row * noiseImage.image.width)] = {(unsigned char) GetRandomValue(0, 255),
                                                                            (unsigned char) GetRandomValue(0, 255),
                                                                            (unsigned char) GetRandomValue(0, 255), 
                                                                            255};
        }
        noiseImage.insert_row++;
        if (noiseImage.insert_row > noiseImage.image.height) {
            noiseImage.insert_row = 0;
        }

        // for now, just re-send the whole texture
        // looks like there are resources for sending subregions in some openGL
        // versions: https://stackoverflow.com/questions/18149967/updating-only-a-horizontal-subregion-of-a-texture-in-opengl-es-2-0
        updateTexture(noiseImage.texture, noiseImage.image);

        /* Render here */
        glClearColor(0.4f, 0.3f, 0.4f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // update texture
        if (!paused) {
            offset[1] += 0.01;
            glUniform2fv(textureOffsetId, 1, offset);
            glUniform2fv(textureScaleId, 1, scale);
            //fillImageWithNoise(noiseImage.image, 0.5f);
            //updateTexture(noiseImage.texture, noiseImage.image);
        }

        // drawing the texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, noiseImage.texture.id);
        ourShader.use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            paused = !paused;
        }
        glfwPollEvents();
    }

    freeTexture(noiseImage.texture);
    free(noiseImage.image.data);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();

    return 0;
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    screenWidth = width;
    screenHeight = height;
    noiseImage.image.width = screenWidth;
    noiseImage.image.height = screenHeight;
    fillImageWithNoise(noiseImage.image, 0.5f);
    updateTexture(noiseImage.texture, noiseImage.image);
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
