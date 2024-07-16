//
// Created by Let'sBlend on 22/06/2024.
//

#ifndef XPLORE_GUI_H
#define XPLORE_GUI_H

#include "imgui.h"
#include "imgui_internal.h"
#include "misc/cpp/imgui_stdlib.h"

class Window;
class GLFWwindow;

namespace ImGui
{
    bool IsWindowRectHovered();
}

class Gui
{
public:
    Gui(GLFWwindow* window);
    ~Gui();

    void StartFrame();
    void Render();
    void EndFrame(Window& window);

    inline static ImFont* m_RegularFont;
    inline static ImFont* m_BoldFont;
};


#endif //XPLORE_GUI_H
