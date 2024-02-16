# Add the library that is built on this subdirectory.
add_library(imgui STATIC imgui/imconfig.h imgui/imgui_draw.cpp imgui/imgui_internal.h imgui/imgui_tables.cpp imgui/imgui_widgets.cpp imgui/imgui.cpp imgui/imgui.h imgui/imstb_rectpack.h imgui/imstb_textedit.h imgui/imstb_truetype.h imgui/backends/imgui_impl_glfw.h imgui/backends/imgui_impl_glfw.cpp imgui/backends/imgui_impl_opengl3.cpp imgui/backends/imgui_impl_opengl3.h)

# target_include_directories(${PROJECT_NAME}.out PRIVATE Adder)
target_include_directories(imgui PUBLIC $(CMAKE_CURRENT_SOURCE_DIR)/imgui ${GLFW_INCLUDE_DIR})