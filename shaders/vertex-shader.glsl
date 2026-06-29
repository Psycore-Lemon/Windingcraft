#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in float aTexLayer;

out vec2 TexCoord;
flat out float TexLayer;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoord = aTexCoord;
    TexLayer = aTexLayer;
    gl_Position = projection * view * model * vec4(aPosition, 1.0);
}
