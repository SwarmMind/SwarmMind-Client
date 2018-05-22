set(sources
    imconfig.h
    imgui.cpp
    imgui.h
    imgui_draw.cpp
    imgui_internal.h
    stb_rect_pack.h
    stb_textedit.h
    stb_truetype.h
    examples/opengl3_example/imgui_impl_glfw_gl3.cpp
    examples/opengl3_example/imgui_impl_glfw_gl3.h)

add_library(imgui STATIC ${sources})
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
