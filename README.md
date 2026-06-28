# Windingcraft

A C++ OpenGL game prototype — an early-stage CRPG built from scratch.

## Tech Stack

- **C++17** with CMake 3.20+
- **OpenGL 4.6** Core Profile
- **GLFW** — windowing and input
- **GLAD** — OpenGL function loader
- **GLM** — math (vectors, matrices, transforms)
- **nlohmann/json** — runtime configuration
- **Dear ImGui** (docking branch) — included in dependencies, not yet wired up

All dependencies are vendored under `dependencies/`.

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

The executable must be run from the project root so it can find `config.json` and `shaders/`.

## Controls

| Key | Action |
|-----|--------|
| W / S | Move forward / backward |
| A / D | Strafe left / right |
| Mouse | Look around |

## Project Structure

```
src/
├── main.cpp              Entry point, window setup, render loop
├── core/
│   ├── Config.h/.cpp     JSON config loader
│   ├── Camera.h          FPS camera (WASD + mouse look)
│   └── Shader.h          Shader program wrapper
└── models/
    └── Cube.h/.cpp       Unit cube mesh (VAO/VBO)

shaders/
├── shader.vert           Vertex shader
└── shader.frag           Fragment shader

config.json               Runtime config (window size, fullscreen)
```

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
