set(sources
    imconfig.h
    imgui.cpp
    imgui.h
    imgui_draw.cpp
    imgui_widgets.cpp
    imgui_internal.h
    imstb_rectpack.h
    imstb_textedit.h
    imstb_truetype.h
    examples/imgui_impl_glfw.cpp
    examples/imgui_impl_glfw.h
    examples/imgui_impl_opengl3.cpp
    examples/imgui_impl_opengl3.h
    misc/stl/imgui_stl.cpp
    misc/stl/imgui_stl.h)

add_library(imgui STATIC ${sources})
target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(imgui PUBLIC glbinding::glbinding glfw)

