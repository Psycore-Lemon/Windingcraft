# Windingcraft

A C++ OpenGL voxel game prototype — chunked procedural terrain, block building/destruction, multiplayer networking, and a wuxia cultivation CRPG in early development.

## Tech Stack

- **C++17** with CMake 3.20+
- **OpenGL 4.6** Core Profile
- **GLFW** — windowing and input
- **GLAD** — OpenGL function loader
- **GLM** — math (vectors, matrices, transforms)
- **nlohmann/json** — runtime config and world saves
- **Dear ImGui** (docking branch) — in-game UI (HUD, menus, hotbar)
- **FastNoiseLite** — procedural terrain noise (header-only)
- **stb_image** — texture loading (header-only)
- **ENet** — UDP networking (vendored)

All dependencies are vendored under `dependencies/`. GLM is a git submodule; the rest are committed directly.

## Building

```bash
cmake -B build -S .
cmake --build build
```

For a release build:

```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

## Running

```bash
./build/Windingcraft
```

The executable must be run from the project root so it can find `config.json`, `shaders/`, and `assets/`.

## Controls

| Input | Action |
|-------|--------|
| W / A / S / D | Move |
| Mouse | Look around |
| Space | Jump |
| Space (double-tap) | Toggle fly mode |
| Space / Shift | Fly up / down |
| Left click | Break block |
| Right click | Place block |
| Scroll wheel | Cycle hotbar |
| 1–9 | Select hotbar slot |
| Escape | Pause / resume |

## Project Structure

```
src/
├── main.cpp                        Entry point
├── core/
│   ├── Engine.h/.cpp               Game loop, state machine (Menu/Playing/Paused)
│   ├── Config.h/.cpp               JSON config loader
│   ├── InputHandler.h/.cpp         Key bindings, mouse/scroll, action enum
│   ├── Time.h/.cpp                 Frame delta time
│   └── Window.h/.cpp               GLFW window lifecycle, GL context
├── game/
│   ├── BlockType.h                 Block enum + definitions (name, solid, texture layer)
│   ├── Inventory.h/.cpp            9-slot hotbar (type + count, scroll/key select)
│   ├── PlayerCommand.h             Serializable input command (client→server)
│   ├── Resource.h                  ResourcePool + Vitals (health, mana, stamina)
│   └── SaveData.h/.cpp             World save/load (JSON + binary chunk files)
├── graphics/
│   ├── Shader.h/.cpp               Shader program (load, compile, uniforms)
│   ├── Mesh.h/.cpp                 Generic mesh (VAO/VBO/EBO, configurable stride)
│   ├── PrimitiveMeshes.h/.cpp      Built-in shapes (cube, etc.)
│   ├── Renderer.h/.cpp             BeginFrame / Draw / DrawBlockHighlight / EndFrame
│   ├── Texture.h/.cpp              Single 2D texture (stb_image)
│   ├── TextureArray.h/.cpp         GL_TEXTURE_2D_ARRAY for block textures
│   ├── WorldRenderer.h/.cpp        Chunk mesh sync and draw
│   └── PlayerRenderer.h/.cpp       Other-player rendering
├── physics/
│   └── AABB.h                      Axis-aligned bounding box (overlap, penetration)
├── scene/
│   ├── Camera.h/.cpp               FPS camera (mouse look, view/projection)
│   ├── Player.h/.cpp               Server-side player (physics, vitals, inventory)
│   ├── PlayerController.h/.cpp     Client-only: camera, command building, server sync
│   ├── PlayerStatus.h              Read-only snapshot for UI
│   └── Transform.h                 Position/rotation/scale with model matrix
├── net/
│   ├── ByteBuffer.h                Serialization helper
│   ├── NetworkTypes.h              Packet types, port, channels
│   ├── NetServer.h/.cpp            ENet host in server mode
│   ├── NetClient.h/.cpp            ENet host in client mode
│   └── PacketSerializer.h/.cpp     Encode/decode game types for the wire
├── server/
│   ├── GameServer.h/.cpp           Authoritative simulation (world, players, ticking)
│   └── HostedServer.h/.cpp         GameServer + NetServer combined
├── ui/
│   ├── UIManager.h/.cpp            ImGui lifecycle + state-based render dispatch
│   ├── HUD.h/.cpp                  FPS, position, mode, target info
│   ├── Hotbar.h/.cpp               9-slot hotbar at screen bottom
│   ├── MainMenu.h/.cpp             Singleplayer / Options / Quit
│   ├── PauseMenu.h/.cpp            Resume / Options / Quit to Menu/Desktop
│   └── VideoSettings.h/.cpp        Resolution and fullscreen settings
└── world/
    ├── World.h/.cpp                Chunk manager, spatial queries, save/load
    ├── Chunk.h/.cpp                16×16×64 block grid
    ├── ChunkMeshBuilder.h/.cpp     Visible-face mesh per chunk
    ├── ChunkSerializer.h/.cpp      Palette-based binary chunk format
    └── TerrainGenerator.h/.cpp     Noise-based heightmap (stone + dirt layers)

assets/
└── textures/                       16×16 block textures (stone.png, dirt.png)

shaders/
├── vertex-shader.glsl              Vertex: position + UV + texture layer index
├── fragment-shader.glsl            Fragment: samples GL_TEXTURE_2D_ARRAY
├── line-vertex.glsl                Block highlight wireframe
└── line-fragment.glsl              Solid white line

config.json                         Runtime config (window size, fullscreen)
saves/                              Per-world save directories (gitignored)
```

## Architecture

Windingcraft uses a **listen server** pattern:

- **Singleplayer** — in-process `GameServer` + local client (direct calls, no network)
- **Multiplayer host** — same `GameServer` + `NetServer` listener for remote clients
- **Multiplayer join** — client only, connecting to a remote host

The boundary between client and server is `PlayerCommand` (client→server) and player state snapshots (server→client). The server ticks at 30 Hz; rendering runs at frame rate.

## Configuration

Edit `config.json` to change window settings:

```json
{
    "window": {
        "fullscreen": false,
        "width": 1280,
        "height": 720
    }
}
```

## License

TBD
