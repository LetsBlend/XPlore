//
// Created by Let'sBlend on 22/06/2024.
//

#ifndef XPLORE_GUI_H
#define XPLORE_GUI_H

#include "imgui.h"

class Window;
class GLFWwindow;

class Gui
{
public:
    Gui(GLFWwindow* window);
    ~Gui();

    void StartFrame();
    void Render();
    void EndFrame(Window& window);
};


#endif //XPLORE_GUI_H
