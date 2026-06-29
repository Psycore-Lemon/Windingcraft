#include "graphics/PlayerRenderer.h"
#include "graphics/Renderer.h"
#include "graphics/Shader.h"
#include "scene/Camera.h"
#include "net/PacketSerializer.h"
#include "game/GameConfig.h"

#include <glm/gtc/matrix_transform.hpp>

static Mesh CreateBoxMesh(float w, float h, float d, float r, float g, float b)
{
    float hw = w * 0.5f, hh = h * 0.5f, hd = d * 0.5f;

    // pos(3) + uv(2) + layer(1) = stride 6
    // We use layer -1 and flat color in a dedicated shader pass,
    // but for now just use the block shader with a dummy layer.
    // Actually, let's use stride 6 with a special layer that maps to a solid color.
    // For simplicity, we'll build stride 8: pos(3) + uv(2) + color(3)
    std::vector<float> verts = {
        // +X
         hw,-hh,-hd, 0,0, r,g,b,   hw, hh,-hd, 1,0, r,g,b,
         hw, hh, hd, 1,1, r,g,b,   hw,-hh, hd, 0,1, r,g,b,
        // -X
        -hw,-hh, hd, 0,0, r,g,b,  -hw, hh, hd, 1,0, r,g,b,
        -hw, hh,-hd, 1,1, r,g,b,  -hw,-hh,-hd, 0,1, r,g,b,
        // +Y
        -hw, hh,-hd, 0,0, r,g,b,   hw, hh,-hd, 1,0, r,g,b,
         hw, hh, hd, 1,1, r,g,b,  -hw, hh, hd, 0,1, r,g,b,
        // -Y
        -hw,-hh, hd, 0,0, r,g,b,   hw,-hh, hd, 1,0, r,g,b,
         hw,-hh,-hd, 1,1, r,g,b,  -hw,-hh,-hd, 0,1, r,g,b,
        // +Z
        -hw,-hh, hd, 0,0, r,g,b,   hw,-hh, hd, 1,0, r,g,b,
         hw, hh, hd, 1,1, r,g,b,  -hw, hh, hd, 0,1, r,g,b,
        // -Z
         hw,-hh,-hd, 0,0, r,g,b,  -hw,-hh,-hd, 1,0, r,g,b,
        -hw, hh,-hd, 1,1, r,g,b,   hw, hh,-hd, 0,1, r,g,b,
    };

    std::vector<unsigned int> idx;
    for (unsigned int f = 0; f < 6; ++f)
    {
        unsigned int base = f * 4;
        idx.push_back(base); idx.push_back(base+1); idx.push_back(base+2);
        idx.push_back(base+2); idx.push_back(base+3); idx.push_back(base);
    }

    return Mesh(verts, idx, 8);
}

PlayerRenderer::PlayerRenderer()
{
    bodyMesh = std::make_unique<Mesh>(CreateBoxMesh(0.5f, 1.2f, 0.3f, 0.2f, 0.5f, 0.8f));
    headMesh = std::make_unique<Mesh>(CreateBoxMesh(0.4f, 0.4f, 0.4f, 0.85f, 0.7f, 0.55f));
}

void PlayerRenderer::Render(const Renderer& renderer, const Shader& shader,
                            const Camera& camera, float aspectRatio,
                            const std::vector<PlayerSnapshot>& players,
                            int localPlayerId)
{
    for (const auto& snap : players)
    {
        if (snap.playerId == localPlayerId)
            continue;

        glm::vec3 pos = snap.position;

        // Body centered at waist height
        glm::mat4 bodyModel = glm::translate(glm::mat4(1.0f),
            pos + glm::vec3(0.0f, 0.6f, 0.0f));
        renderer.Draw(*bodyMesh, shader, camera, bodyModel, aspectRatio);

        // Head on top of body
        glm::mat4 headModel = glm::translate(glm::mat4(1.0f),
            pos + glm::vec3(0.0f, 1.4f, 0.0f));
        renderer.Draw(*headMesh, shader, camera, headModel, aspectRatio);
    }
}
