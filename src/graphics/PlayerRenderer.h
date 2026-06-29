#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <memory>

#include "graphics/Mesh.h"

class Shader;
class Camera;
class Renderer;
struct PlayerSnapshot;

class PlayerRenderer
{
public:
    PlayerRenderer();

    void Render(const Renderer& renderer, const Shader& shader,
                const Camera& camera, float aspectRatio,
                const std::vector<PlayerSnapshot>& players,
                int localPlayerId);

private:
    std::unique_ptr<Mesh> bodyMesh;
    std::unique_ptr<Mesh> headMesh;
};
