#version 330 core

in vec2 EdgeCoord;
in vec3 BlockColor;
out vec4 FragColor;

void main()
{
    float borderWidth = 0.01;

    bool onEdge = EdgeCoord.x < borderWidth || EdgeCoord.x > 1.0 - borderWidth ||
                  EdgeCoord.y < borderWidth || EdgeCoord.y > 1.0 - borderWidth;

    vec3 edgeColor = BlockColor * 0.3;

    FragColor = vec4(onEdge ? edgeColor : BlockColor, 1.0);
}
