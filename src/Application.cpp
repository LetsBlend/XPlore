//
// Created by Let'sBlend on 22/06/2024.
//

#include "Application.h"

#include "Gui/Gui.h"
#include "Gui/GuiTemplate.h"
#include "Window/Window.h"

void Application::Update(Window& window, Gui& gui)
{
    window.PollEvents(m_Running);

    gui.StartFrame();

    WindowDocking();
    ExampleWindow();

    ImGui::Begin("Hierarchy");

    bool itemClicked = false;
    int currentNameIndex = 0;
    int gStartFlag = ImGuiTreeNodeFlags_DefaultOpen;
    m_HirarchyView.DisplayHirarchy(itemClicked, m_Root, m_XPManager, m_PopUpView, currentNameIndex, gStartFlag);
    ImGui::End();

    ImGui::Begin("Directory");

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