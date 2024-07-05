cmake_minimum_required(VERSION 3.26)
project(XPlore)

set(CMAKE_CXX_STANDARD 23)
include_directories(src vendor/glfw/include vendor/glad/include vendor/imgui vendor/imgui/backends)
add_subdirectory(vendor/glfw)
add_subdirectory(vendor/glad)
add_subdirectory(vendor/imgui)

add_compile_definitions(GLFW_INCLUDE_NONE)

add_executable(XPlore src/main.cpp
        src/Application.cpp
        src/Application.h
        src/Window/Window.cpp
        src/Window/Window.h
        src/Gui/Gui.cpp
        src/Gui/Gui.h
        src/Gui/GuiTemplate.cpp
        src/Gui/GuiTemplate.h
        src/XPloreManager.cpp
        src/XPloreManager.h
        src/Util/Fonts.cpp
        src/Util/Fonts.h
        src/Util/ConsoleColors.h
        src/Debug/Debugger.h
        src/Util/StringConverter.h
        src/HirarchyView.cpp
        src/HirarchyView.h
        src/DirectoryView.cpp
        src/DirectoryView.h
        src/PopUpView.cpp
        src/PopUpView.h
        src/Gui/CmGui.cpp
        src/Gui/CmGui.h
        src/Debug/Timer.cpp
        src/Debug/Timer.h
)

target_precompile_headers(XPlore PRIVATE src/pch.h)
target_link_libraries(XPlore PRIVATE glfw glad imgui stdc++fs)