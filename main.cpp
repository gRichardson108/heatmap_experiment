#include <iostream>
#include <cstring>
#include <malloc.h>

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include "rendering.h"
#include "shader.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

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

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
    }
}
void GLAPIENTRY OpenGlDebugMessageCallback(GLenum source,
                                           GLenum type,
                                           GLuint id,
                                           GLenum severity,
                                           GLsizei length,
                                           const GLchar* message,
                                           const void* userParam) {
    const char* severity_str;


#define ENUM_TO_STR_CASE(sym, name) \
    case (name):                 \
        (sym) = #name;  \
        break;

    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;

    switch (severity) {
        ENUM_TO_STR_CASE(severity_str, GL_DEBUG_SEVERITY_LOW)
        ENUM_TO_STR_CASE(severity_str, GL_DEBUG_SEVERITY_MEDIUM)
        ENUM_TO_STR_CASE(severity_str, GL_DEBUG_SEVERITY_HIGH)
        ENUM_TO_STR_CASE(severity_str, GL_DEBUG_SEVERITY_NOTIFICATION)
        default:
            severity_str = "Unknown";
            break;
    }
    fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = %s, message = %s\n",
             ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
             type, severity_str, message );
}

static void printGLError() {
    GLenum gl_err = glGetError();
    if (gl_err != 0)
        fprintf(stderr, "%s:%d GL error 0x%x \n", __FILE__, __LINE__, gl_err);
}

void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        } else if (action == GLFW_RELEASE) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
}

void mouseMovementCallback(GLFWwindow *window, double xpos, double ypos) {
}
static void glfw_error_callback(int error, const char *description) {
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

int main(void) {
    GLFWwindow *window;
    glfwSetErrorCallback(glfw_error_callback);
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char *glsl_version = "#version 330";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(screenWidth, screenHeight, "Noise renderer", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, mouseMovementCallback);
    glfwSwapInterval(1); // Enable vsync
    
    // setup GLAD
    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0) {
        fprintf(stderr, "Failed to initialize OpenGL context\n");
        return 1;
    }
    std::string versionString = std::string((const char*)glGetString(GL_VERSION));
    std::cout << "Version: " << versionString << std::endl;
    
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE);
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(OpenGlDebugMessageCallback, 0);

    printGLError();
   
    // Setup Dear ImGui context
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    // ImGui demo window state
    bool show_demo_window = false;
    
    noiseImage = {
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
    GL_CALL(glGenVertexArrays(1, &VAO));
    GL_CALL(glGenBuffers(1, &VBO));
    GL_CALL(glGenBuffers(1, &EBO));

    GL_CALL(glBindVertexArray(VAO));

    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
    GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

    // position attribute
    GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0));
    GL_CALL(glEnableVertexAttribArray(0));
    // color attribute
    GL_CALL(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float))));
    GL_CALL(glEnableVertexAttribArray(1));
    // texture coord attribute
    GL_CALL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float))));
    GL_CALL(glEnableVertexAttribArray(2));

    Shader ourShader("resources/texture.vsh", "resources/texture.fsh");
    ourShader.use();
    GL_CALL(glUniform1i(glGetUniformLocation(ourShader.ID, "texture1"), 0));
    auto textureOffsetId = glGetUniformLocation(ourShader.ID, "textureOffset");
    auto textureScaleId = glGetUniformLocation(ourShader.ID, "textureScale");

    /* Loop until the user closes the window */
    GLfloat offset[2] = {};
    GLfloat scale[2] = {1.0f, 1.0f};
    GL_CALL(glUniform2fv(textureOffsetId, 1, offset));
    GL_CALL(glUniform2fv(textureScaleId, 1, scale));
    bool paused = true;
    int scanSpeed = 1;
    int redRange[2] = {0, 255};
    int greenRange[2] = {0, 255};
    int blueRange[2] = {0, 255};
    
    while (!glfwWindowShouldClose(window)) {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        if (!io.WantCaptureKeyboard) {
            processInput(window);
        }

        //====================================
        //====== Rendering for noise =========

        // generate a random row of data
        ColorRGBA *pixels = (ColorRGBA *) noiseImage.image.data;
        for (int scan = 0; scan < scanSpeed; scan++) {
            for (int i = 0; i < screenWidth; i++) {
                pixels[i + (noiseImage.insert_row * noiseImage.image.width)] = {(unsigned char) GetRandomValue(redRange[0], redRange[1]),
                                                                                (unsigned char) GetRandomValue(greenRange[0], greenRange[1]),
                                                                                (unsigned char) GetRandomValue(blueRange[0], blueRange[1]),
                                                                                255};
            }
            noiseImage.insert_row++;
            if (noiseImage.insert_row > noiseImage.image.height) {
                noiseImage.insert_row = 0;
            }
        }

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        GL_CALL(glViewport(0, 0, display_w, display_h));

        // for now, just re-send the whole texture
        // looks like there are resources for sending subregions in some openGL
        // versions: https://stackoverflow.com/questions/18149967/updating-only-a-horizontal-subregion-of-a-texture-in-opengl-es-2-0
        updateTexture(noiseImage.texture, noiseImage.image);

        /* Render here */
        GL_CALL(glClearColor(0.4f, 0.3f, 0.4f, 0.0f));
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        // update texture
        if (!paused) {
            offset[1] += 0.01;
            GL_CALL(glUniform2fv(textureOffsetId, 1, offset));
            GL_CALL(glUniform2fv(textureScaleId, 1, scale));
        }
        // drawing the texture
        GL_CALL(glActiveTexture(GL_TEXTURE0));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, noiseImage.texture.id));
        ourShader.use();
        GL_CALL(glBindVertexArray(VAO));
        GL_CALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
        GL_CALL(glBindVertexArray(0));
        glUseProgram(0);
        GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
        {
            ImGui::Begin("Scan Settings");
            ImGui::Text(
                    "Adjust scan settings.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::SliderInt("Scan Speed", &scanSpeed, 1, 10);
            ImGui::SliderInt2("Red", redRange, 0, 255);
            ImGui::SliderInt2("Green", greenRange, 0, 255);
            ImGui::SliderInt2("Blue", blueRange, 0, 255);
            
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                        ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // =============================
        // ==== Rendering for ImGui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    freeTexture(noiseImage.texture);
    free(noiseImage.image.data);

    GL_CALL(glDeleteVertexArrays(1, &VAO));
    GL_CALL(glDeleteBuffers(1, &VBO));
    GL_CALL(glDeleteBuffers(1, &EBO));

    glfwDestroyWindow(window);
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
