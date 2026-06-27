#version 330 core
out vec4 FragColor;

in vec3 vColor;
in vec2 TexCoord;           // available, just not sampled yet

void main()
{
    FragColor = vec4(vColor, 1.0);
}