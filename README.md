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

The executable must be run from the project root so it can find `config.json` and `shaders/`.

## Controls

| Key | Action |
|-----|--------|
| W / S | Move forward / backward |
| A / D | Strafe left / right |
| Space | Jump |
| Escape | Quit |
| Mouse | Look around |

## Project Structure

```
src/
├── main.cpp                    Entry point, window setup, game loop
├── core/
│   ├── Config.h/.cpp           JSON config loader
│   ├── InputHandler.h/.cpp     Key bindings, mouse delta, action mapping
│   └── Time.h                  Frame delta time
├── graphics/
│   ├── Shader.h/.cpp           Shader program (load, compile, uniforms)
│   ├── Mesh.h/.cpp             Generic mesh (VAO/VBO/EBO)
│   ├── PrimitiveMeshes.h/.cpp  Factory for built-in shapes (cube, triangle)
│   └── Renderer.h/.cpp         BeginFrame / Draw / EndFrame abstraction
└── scene/
    └── Camera.h/.cpp           FPS camera (WASD + mouse look)

shaders/
├── vertex-shader.glsl          Vertex shader
└── fragment-shader.glsl        Fragment shader

config.json                     Runtime config (window size, fullscreen)
```

> **Note:** Anything that touches OpenGL directly lives in `graphics/`. Other modules (core, scene) should not include `<glad/gl.h>` or call `gl*` functions.

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
