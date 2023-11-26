#ifndef SOCKETHEATMAPDISPLAY_RENDERING_H
#define SOCKETHEATMAPDISPLAY_RENDERING_H

/// Vector2 type
typedef struct Vector2 {
    float x;
    float y;
} Vector2;

/// Vector3 type
typedef struct Vector3 {
    float x;
    float y;
    float z;
} Vector3;

/// Color type, RGBA (32bit)
struct ColorRGBA {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

struct ColorRGB {
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

namespace RGBA {
    const ColorRGBA WHITE = {255, 255, 255, 255};
    const ColorRGBA BLACK = {0, 0, 0, 255};
    const ColorRGBA RED = {255, 0, 0, 255};
    const ColorRGBA BLUE = {0, 0, 255, 255};
};
namespace RGB {
    const ColorRGB WHITE = {255, 255, 255};
    const ColorRGB BLACK = {0, 0, 0};
    const ColorRGB RED = {255, 0, 0};
    const ColorRGB BLUE = {0, 0, 255};
};

/// Image, pixel data stored in CPU memory (RAM)
struct Image {
    /// Image raw data (GL_RGB8)
    void *data;
    /// Image base width
    int width;
    /// Image base height
    int height;
};

/// Texture, tex data stored in GPU memory (VRAM)
struct Texture2D {
    /// OpenGL texture id
    unsigned int id;
    /// Texture base width
    int width;
    /// Texture base height
    int height;
};

// Matrix type (OpenGL style 4x4 - right handed, column major)
typedef struct Matrix {
    float m0, m4, m8, m12;      // Matrix first row (4 components)
    float m1, m5, m9, m13;      // Matrix second row (4 components)
    float m2, m6, m10, m14;     // Matrix third row (4 components)
    float m3, m7, m11, m15;     // Matrix fourth row (4 components)
} Matrix;

// NOTE: Helper types to be used instead of array return types for *ToFloat functions
typedef struct float3 {
    float v[3];
} float3;

typedef struct float16 {
    float v[16];
} float16;


float16 MatrixToFloatV(Matrix mat);

Matrix MatrixIdentity(void);

/// Allocate a texture object from image data.
/// \param image Image data to use for the texture
/// \return texture object created from the image. Texture needs to be freed via freeTexture later.
/// On error, the texture object has an id of 0.
Texture2D allocateTexture(Image image);

/// Updates an existing texture's data. Texture must have same dimensions as image.
/// \param texture Texture to update.
/// \param image Image data to apply to the texture.
void updateTexture(Texture2D& texture, const Image& image);

/// Frees a texture that was allocated with allocateTexture.
/// \param texture The texture to free.
void freeTexture(Texture2D texture);

/// Get a random value between min and max (both included)
/// \param min Minimum value [inclusive].
/// \param max Maximum value [inclusive].
/// \returns Random value.
int GetRandomValue(int min, int max);

/// Overwrites an image with white noise data. Image must already be allocated.
/// \param image The image to fill with noise data.
/// \param factor Percentage of noise to create as white
void fillImageWithNoise(Image& image, float factor);

/// Overwrites an image with horizontal black and white stripes.
/// \param image The image to fill with stripes.
/// \param width The width of the stripes
void fillImageWithHorizontalStripes(Image& image, int width);

/// Oversimplified function that draws the texture over an identical screen space.
/// \param texture The texture to draw.
void drawTexture(Texture2D texture);

#endif //SOCKETHEATMAPDISPLAY_RENDERING_H
