//
// Created by garret on 11/18/22.
//

#include <GL/gl.h>
#include <cstdlib>
#include "rendering.h"
#include <iostream>


Texture2D allocateTexture(Image image) {
    Texture2D texture = {0};
    if (image.width != 0 && image.height != 0) {
        unsigned int id = 0;
        glBindTexture(GL_TEXTURE_2D, 0);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        glGenTextures(1, &id);              // Generate texture id
        texture.id = id;

        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width, image.height,
                     0, GL_RGB, GL_UNSIGNED_BYTE, (unsigned char *) image.data);

        // Texture parameters configuration
        // NOTE: glTexParameteri does NOT affect texture uploading, just the way it's used
#if defined(GRAPHICS_API_OPENGL_ES2)
        // NOTE: OpenGL ES 2.0 with no GL_OES_texture_npot support (i.e. WebGL) has limited NPOT support, so CLAMP_TO_EDGE must be used
    if (RLGL.ExtSupported.texNPOT)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);       // Set texture to repeat on x-axis
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);       // Set texture to repeat on y-axis
    }
    else
    {
        // NOTE: If using negative texture coordinates (LoadOBJ()), it does not work!
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);       // Set texture to clamp on x-axis
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);       // Set texture to clamp on y-axis
    }
#else
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);       // Set texture to repeat on x-axis
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);       // Set texture to repeat on y-axis
#endif

        // Magnification and minification filters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        // Unbind current texture
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    return texture;
}

void freeTexture(Texture2D texture) {
    glDeleteTextures(1, &texture.id);
}


int GetRandomValue(int min, int max) {
    if (min > max) {
        int tmp = max;
        max = min;
        min = tmp;
    }

    return (rand() % (abs(max - min) + 1) + min);
}

void fillImageWithNoise(Image &image, float factor) {
    Color *pixels = (Color *) image.data;
    for (int i = 0; i < image.width * image.height; i++) {
        if (GetRandomValue(0, 99) < (int) (factor * 100.0f)) pixels[i] = WHITE;
        else pixels[i] = BLACK;
    }
}

void fillImageWithHorizontalStripes(Image &image, int width) {
    Color *pixels = (Color *) image.data;
    for (int i = 0; i < image.height; i++) {
        for (int j = 0; j < image.width; j++) {
            // todo
        }
    }
}

void drawTexture(Texture2D texture) {
    Vector2 topLeft = {0, 0};
    Vector2 topRight = {(float) texture.width, 0};
    Vector2 bottomLeft = {0, (float) texture.height};
    Vector2 bottomRight = {(float) texture.width, (float) texture.height};

    glBegin(GL_QUADS);

    glColor4ub(WHITE.r, WHITE.g, WHITE.b, WHITE.a);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture.id);

    // Set texture coordinates for top-left vertex
    glTexCoord2f(0, 0);
    glVertex2f(topLeft.x, topLeft.y);

    // Set texture coordinates for bottom-left vertex
    glTexCoord2f(0, 1);
    glVertex2f(bottomLeft.x, bottomLeft.y);

    // Set texture coordinates for bottom-right vertex
    glTexCoord2f(1, 1);
    glVertex2f(bottomRight.x, bottomRight.y);

    // Set texture coordinates for top-right vertex
    glTexCoord2f(1, 0);
    glVertex2f(topRight.x, topRight.y);

    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
}

void updateTexture(Texture2D &texture, const Image &image) {
    if (image.width != texture.width && image.height != texture.height) {
        glBindTexture(GL_TEXTURE_2D, texture.id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.width, image.height,
                     0, GL_RGB, GL_UNSIGNED_BYTE, (unsigned char *) image.data);

        // Texture parameters configuration
        // NOTE: glTexParameteri does NOT affect texture uploading, just the way it's used
#if defined(GRAPHICS_API_OPENGL_ES2)
        // NOTE: OpenGL ES 2.0 with no GL_OES_texture_npot support (i.e. WebGL) has limited NPOT support, so CLAMP_TO_EDGE must be used
    if (RLGL.ExtSupported.texNPOT)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);       // Set texture to repeat on x-axis
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);       // Set texture to repeat on y-axis
    }
    else
    {
        // NOTE: If using negative texture coordinates (LoadOBJ()), it does not work!
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);       // Set texture to clamp on x-axis
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);       // Set texture to clamp on y-axis
    }
#else
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);       // Set texture to repeat on x-axis
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);       // Set texture to repeat on y-axis
#endif

        // Magnification and minification filters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  // Alternative: GL_LINEAR
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  // Alternative: GL_LINEAR
        // Unbind current texture
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

Matrix MatrixIdentity(void) {
    Matrix result = {1.0f, 0.0f, 0.0f, 0.0f,
                     0.0f, 1.0f, 0.0f, 0.0f,
                     0.0f, 0.0f, 1.0f, 0.0f,
                     0.0f, 0.0f, 0.0f, 1.0f};

    return result;
}

float16 MatrixToFloatV(Matrix mat) {
    float16 result = {0};

    result.v[0] = mat.m0;
    result.v[1] = mat.m1;
    result.v[2] = mat.m2;
    result.v[3] = mat.m3;
    result.v[4] = mat.m4;
    result.v[5] = mat.m5;
    result.v[6] = mat.m6;
    result.v[7] = mat.m7;
    result.v[8] = mat.m8;
    result.v[9] = mat.m9;
    result.v[10] = mat.m10;
    result.v[11] = mat.m11;
    result.v[12] = mat.m12;
    result.v[13] = mat.m13;
    result.v[14] = mat.m14;
    result.v[15] = mat.m15;

    return result;
}
