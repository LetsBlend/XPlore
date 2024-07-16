//
// Created by Let'sBlend on 22/06/2024.
//

#ifndef XPLORE_APPLICATION_H
#define XPLORE_APPLICATION_H

#include "Render/HirarchyView.h"
#include "Render/DirectoryView.h"
#include "Render/PopUpView.h"
#include "XPloreManager.h"

class Window;
class Gui;

class Application
{
public:
    Application() = default;
    ~Application() = default;

    bool IsRunning();
    void ReadDrives();
    void Update(Window& window, Gui& gui);

private:
    bool m_Running = true;
    HirarchyView m_HirarchyView;
    DirectoryView m_DirectoryView;
    PopUpView m_PopUpView;
    XPloreManager m_XPManager;
    std::vector<Directory> m_Roots;
};


#endif //XPLORE_APPLICATION_H
