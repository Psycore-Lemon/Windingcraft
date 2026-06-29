#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aColor;

out vec3 VertColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    VertColor = aColor;
    gl_Position = projection * view * model * vec4(aPosition, 1.0);
}
