//
// Created by garret on 11/18/22.
//

#include <glad/gl.h>
#include <cstdlib>
#include "rendering.h"
#include <iostream>

Texture2D allocateTexture(Image image) {
    Texture2D texture = {0};
    if (image.width != 0 && image.height != 0) {
        unsigned int id = 0;
        GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
        GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

        GL_CALL(glGenTextures(1, &id));              // Generate texture id
        texture.id = id;

        GL_CALL(glBindTexture(GL_TEXTURE_2D, id));
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height,
                     0, GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char *) image.data));

        // Texture parameters configuration
        // NOTE: glTexParameteri does NOT affect texture uploading, just the way it's used
#if defined(GRAPHICS_API_OPENGL_ES2)
        // NOTE: OpenGL ES 2.0 with no GL_OES_texture_npot support (i.e. WebGL) has limited NPOT support, so CLAMP_TO_EDGE must be used
    if (RLGL.ExtSupported.texNPOT)
    {
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));       // Set texture to repeat on x-axis
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));       // Set texture to repeat on y-axis
    }
    else
    {
        // NOTE: If using negative texture coordinates (LoadOBJ()), it does not work!
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));       // Set texture to clamp on x-axis
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));       // Set texture to clamp on y-axis
    }
#else
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));       // Set texture to repeat on x-axis
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));       // Set texture to repeat on y-axis
#endif

        // Magnification and minification filters
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        // Unbind current texture
        GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    }
    return texture;
}

void freeTexture(Texture2D texture) {
    GL_CALL(glDeleteTextures(1, &texture.id));
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
    ColorRGBA *pixels = (ColorRGBA *) image.data;
    for (int i = 0; i < image.width * image.height; i++) {
        if (GetRandomValue(0, 99) < (int) (factor * 100.0f)) pixels[i] = RGBA::RED;
        else pixels[i] = RGBA::BLUE;
    }
}

void updateTexture(Texture2D &texture, const Image &image) {
    if (image.width != texture.width && image.height != texture.height) {
        GL_CALL(glActiveTexture(GL_TEXTURE0));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, texture.id));
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height,
                     0, GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char *) image.data));

        // Texture parameters configuration
        // NOTE: glTexParameteri does NOT affect texture uploading, just the way it's used
#if defined(GRAPHICS_API_OPENGL_ES2)
        // NOTE: OpenGL ES 2.0 with no GL_OES_texture_npot support (i.e. WebGL) has limited NPOT support, so CLAMP_TO_EDGE must be used
    if (RLGL.ExtSupported.texNPOT)
    {
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));       // Set texture to repeat on x-axis
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));       // Set texture to repeat on y-axis
    }
    else
    {
        // NOTE: If using negative texture coordinates (LoadOBJ()), it does not work!
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));       // Set texture to clamp on x-axis
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));       // Set texture to clamp on y-axis
    }
#else
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));       // Set texture to repeat on x-axis
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));       // Set texture to repeat on y-axis
#endif

        // Magnification and minification filters
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));  // Alternative: GL_LINEAR
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));  // Alternative: GL_LINEAR
        // Unbind current texture
        GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
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
