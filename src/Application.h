//
// Created by Let'sBlend on 22/06/2024.
//

#ifndef XPLORE_APPLICATION_H
#define XPLORE_APPLICATION_H

#include "HirarchyView.h"
#include "DirectoryView.h"
#include "PopUpView.h"
#include "XPloreManager.h"

class Window;
class Gui;

class Application
{
public:
    Application() = default;
    ~Application() = default;

    bool IsRunning();
    void Update(Window& window, Gui& gui);

private:
    bool m_Running = true;
    HirarchyView m_HirarchyView;
    DirectoryView m_DirectoryView;
    PopUpView m_PopUpView;
    XPloreManager m_XPManager;
    Directory m_Root{"C:\\", "C:", true};
};


#endif //XPLORE_APPLICATION_H
