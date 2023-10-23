cmake_minimum_required(VERSION 3.24)
 
project(ROPE_MAN)
  
set(CMAKE_CXX_STANDARD 17)

# Find OpenMP package
find_package(OpenMP REQUIRED)

add_subdirectory(src/thirdparties/glm)
add_subdirectory(src/thirdparties/glad)
add_subdirectory(src/thirdparties/glfw)
add_subdirectory(src/thirdparties/imgui)
### IMGUI

### END IMGUI

file(GLOB SOURCE_FILES
    "src/*.cpp"
    "src/*.hpp"
    "src/*.h"
)
file(GLOB HELPERS_FILES
    "src/helpers/*.cpp"
    "src/helpers/*.hpp"
    "src/helpers/*.h"
)
file(GLOB SHADER_FILES
    "shaders/*.glsl"
)

add_executable(Rope_Man 
${SOURCE_FILES}
${HELPERS_FILES}
${SHADER_FILES})

# Add the directory of include files
include_directories(src/thirdparties)

target_compile_options(Rope_Man PRIVATE ${OpenMP_CXX_FLAGS})
target_link_libraries(
        Rope_Man
        imgui
        glm
        glad
        glfw
        ${OpenMP_CXX_FLAGS}
)
 
source_group("sources" FILES ${SOURCE_FILES})
source_group("helpers" FILES ${HELPERS_FILES})
source_group("shaders" FILES ${SHADER_FILES}) 
