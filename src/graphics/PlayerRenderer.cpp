#include "graphics/PlayerRenderer.h"
#include "graphics/Renderer.h"
#include "graphics/Shader.h"
#include "scene/Camera.h"
#include "net/PacketSerializer.h"
#include "game/GameConfig.h"

#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>

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

void PlayerRenderer::RenderNameTags(const Camera& camera, float aspectRatio,
                                    const std::vector<PlayerSnapshot>& players,
                                    int localPlayerId)
{
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 proj = camera.GetProjectionMatrix(aspectRatio);
    ImGuiIO& io = ImGui::GetIO();
    ImDrawList* draw = ImGui::GetForegroundDrawList();

    for (const auto& snap : players)
    {
        if (snap.playerId == localPlayerId || snap.username.empty())
            continue;

        glm::vec3 tagPos = snap.position + glm::vec3(0.0f, 1.9f, 0.0f);
        glm::vec4 clip = proj * view * glm::vec4(tagPos, 1.0f);

        if (clip.w <= 0.0f)
            continue;

        glm::vec3 ndc = glm::vec3(clip) / clip.w;

        float screenX = (ndc.x * 0.5f + 0.5f) * io.DisplaySize.x;
        float screenY = (1.0f - (ndc.y * 0.5f + 0.5f)) * io.DisplaySize.y;

        ImVec2 textSize = ImGui::CalcTextSize(snap.username.c_str());
        ImVec2 textPos(screenX - textSize.x * 0.5f, screenY - textSize.y);

        draw->AddRectFilled(
            ImVec2(textPos.x - 3, textPos.y - 1),
            ImVec2(textPos.x + textSize.x + 3, textPos.y + textSize.y + 1),
            IM_COL32(0, 0, 0, 140), 2.0f);

        draw->AddText(textPos, IM_COL32(255, 255, 255, 240), snap.username.c_str());
    }
}
