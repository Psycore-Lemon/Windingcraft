#version 330 core

in vec2 TexCoord;
flat in float TexLayer;
out vec4 FragColor;

uniform sampler2DArray blockTextures;

void main()
{
    FragColor = texture(blockTextures, vec3(TexCoord, TexLayer));
}
