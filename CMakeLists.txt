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
# Executable
# -------------------------

add_executable(Windingcraft
    src/main.cpp
    src/core/Config.cpp
    src/core/Config.h
    src/models/Cube.cpp
)

target_link_libraries(Windingcraft PRIVATE
    glfw
    glad
    json
    glm::glm
)