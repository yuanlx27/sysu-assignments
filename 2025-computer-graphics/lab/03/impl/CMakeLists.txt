cmake_minimum_required(VERSION 3.14)
project(CGHW3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# GLFW
FetchContent_Declare(
  glfw
  GIT_REPOSITORY https://github.com/glfw/glfw.git
  GIT_TAG        3.3.8
)
FetchContent_MakeAvailable(glfw)

# ImGui
FetchContent_Declare(
  imgui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG        v1.89.9
)
FetchContent_MakeAvailable(imgui)

# Define sources for ImGui
set(IMGUI_DIR ${imgui_SOURCE_DIR})
set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

add_executable(CGHW3 src/main.cpp ${IMGUI_SOURCES})

target_include_directories(CGHW3 PRIVATE 
    ${IMGUI_DIR} 
    ${IMGUI_DIR}/backends
)

target_link_libraries(CGHW3 PRIVATE glfw)

# Link OpenGL
if(WIN32)
    target_link_libraries(CGHW3 PRIVATE opengl32)
elseif(APPLE)
    target_link_libraries(CGHW3 PRIVATE "-framework OpenGL")
else()
    target_link_libraries(CGHW3 PRIVATE GL)
endif()
