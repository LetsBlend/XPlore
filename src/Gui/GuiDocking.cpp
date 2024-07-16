//
// Created by Let'sBlend on 10/07/2024.
//

#include "GuiDocking.h"
#include "Gui.h"
#include "imgui_internal.h"

void GuiDocking::InitDockSpace()
{
    static bool p_open = true;
    static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", &p_open, windowFlags);
    ImGui::PopStyleVar();

    ImGui::PopStyleVar(2);

    ImGuiID dockspaceId = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspaceFlags);

    // Build DockSpace
    static auto firstTime = true;
    if (firstTime)
    {
        firstTime = false;

        ImGui::DockBuilderRemoveNode(dockspaceId); // clear any previous layout
        ImGui::DockBuilderAddNode(dockspaceId, dockspaceFlags | ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspaceId, viewport->Size);

        // Set which dockspaceId should correspond to which window and set its position
        auto dockIdLeft = ImGui::DockBuilderSplitNode(dockspaceId, ImGuiDir_Left, 0.2f, nullptr, &dockspaceId);
        auto dockIdLeft2 = ImGui::DockBuilderSplitNode(dockspaceId, ImGuiDir_Left, 0.65f, nullptr, &dockspaceId);
        auto dockIdRight = ImGui::DockBuilderSplitNode(dockspaceId, ImGuiDir_Right, 0.25f, nullptr, &dockspaceId);
        auto dockIdDown = ImGui::DockBuilderSplitNode(dockspaceId, ImGuiDir_Down, 0.5f, nullptr, &dockIdRight);

        // we now dock our windows into the docking node we made above
        ImGui::DockBuilderDockWindow("Hierarchy", dockIdLeft);
        ImGui::DockBuilderDockWindow("Directory", dockIdLeft2);
        ImGui::DockBuilderDockWindow("Dear ImGui Demo", dockIdRight);
        ImGui::DockBuilderDockWindow("Hello, world!", dockIdDown);
        ImGui::DockBuilderFinish(dockspaceId);
    }

    ImGui::End();
}
