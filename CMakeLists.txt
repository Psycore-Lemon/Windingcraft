cmake_minimum_required(VERSION 3.20)

project(Windingcraft LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -------------------------
# Dependencies
# -------------------------

# GLFW
add_subdirectory(dependencies/glfw)

# GLM
add_subdirectory(dependencies/glm)

# GLAD
add_library(glad
    dependencies/glad/src/gl.c
)

target_include_directories(glad PUBLIC
    dependencies/glad/include
)

# JSON
set(NLOHMANN_JSON_TARGET_NAME json)
add_subdirectory(dependencies/json)

# -------------------------
# Source Files
# -------------------------

set(CORE_SOURCES
    src/core/Config.cpp
    src/core/Config.h
    src/core/Time.h
    src/core/InputHandler.cpp
    src/core/InputHandler.h
)

set(GRAPHICS_SOURCES
    src/graphics/Shader.cpp
    src/graphics/Shader.h
    src/graphics/Mesh.cpp
    src/graphics/Mesh.h
    src/graphics/PrimitiveMeshes.cpp
    src/graphics/PrimitiveMeshes.h
    src/rendering/Renderer.cpp
    src/rendering/Renderer.h
)

set(SCENE_SOURCES
    src/scene/Camera.cpp
    src/scene/Camera.h
)

# -------------------------
# Executable
# -------------------------

add_executable(Windingcraft
    src/main.cpp
    ${CORE_SOURCES}
    ${GRAPHICS_SOURCES}
    ${SCENE_SOURCES}
    ${WORLD_SOURCES}
)

# -------------------------
# Include Directories
# -------------------------

target_include_directories(Windingcraft PRIVATE
    src
)

# -------------------------
# Link Libraries
# -------------------------

target_link_libraries(Windingcraft PRIVATE
    glfw
    glad
    json
    glm::glm
)
