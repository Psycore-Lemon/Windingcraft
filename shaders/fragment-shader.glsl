#version 330 core

in vec2 EdgeCoord;
out vec4 FragColor;

void main()
{
    float borderWidth = 0.01;

    bool onEdge = EdgeCoord.x < borderWidth || EdgeCoord.x > 1.0 - borderWidth ||
                  EdgeCoord.y < borderWidth || EdgeCoord.y > 1.0 - borderWidth;

    vec3 faceColor = vec3(0.8, 0.2, 0.2);
    vec3 edgeColor = vec3(0.1, 0.1, 0.1);

    FragColor = vec4(onEdge ? edgeColor : faceColor, 1.0);
}
