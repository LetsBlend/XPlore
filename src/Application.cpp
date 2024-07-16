//
// Created by Let'sBlend on 22/06/2024.
//

#include "Application.h"

#include "Gui/Gui.h"
#include "Gui/GuiDocking.h"
#include "Gui/GuiTemplate.h"
#include "Window/Window.h"

void Application::ReadDrives()
{
    DWORD drivesBitmask = GetLogicalDrives();
    if (drivesBitmask == 0)
        Debug::Error("Retrieving drive information");

    // Iterate over each bit in the bitmask
    for (int i = 0; i < 26; ++i) {
        if (drivesBitmask & (1 << i)) {
            // Construct the drive letter (e.g., "C:\")
            char driveLetter = 'A' + i;
            std::string driveName(1, driveLetter);
            m_Roots.push_back({driveName + ":\\", driveName + ':', true});
        }
    }
}

void Application::Update(Window& window, Gui& gui)
{
    window.PollEvents(m_Running);

    gui.StartFrame();

    GuiDocking::InitDockSpace();
    if(ImGui::IsKeyDown(ImGuiKey_F5))
    {
        m_HirarchyView.m_Refresh = true;
        m_DirectoryView.m_Refresh = true;
    }

    // Render
    ExampleWindow();

    ImGui::Begin("Hierarchy");

    bool itemClicked = false;
    int currentNameIndex = 0;
    int gStartFlag = ImGuiTreeNodeFlags_DefaultOpen;

    for(Directory& root : m_Roots)
        m_HirarchyView.DisplayHirarchy(itemClicked, root, m_XPManager, m_PopUpView, currentNameIndex, gStartFlag);
    m_HirarchyView.m_Refresh = false;
    ImGui::End();

    ImGui::Begin("Directory");

    m_DirectoryView.DisplayLoadingCircle();
    m_DirectoryView.DisplayFilePath(m_XPManager);
    m_DirectoryView.DisplayDirectory(m_XPManager, m_PopUpView);

    ImGui::End();

    m_PopUpView.DisplayPopUp(m_XPManager, m_HirarchyView, m_DirectoryView);

    gui.Render();
    window.SwapBuffers();
    window.Clear();
    gui.EndFrame(window);
}

bool Application::IsRunning()
{
    return m_Running;
}
