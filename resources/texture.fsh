#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

// texture samplers
uniform sampler2D texture1;
uniform vec2 textureOffset;
uniform vec2 textureScale;

void main()
{
    FragColor = texture(texture1, (TexCoord + textureOffset) * textureScale);
}