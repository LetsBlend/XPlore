//
// Created by Let'sBlend on 28/06/2024.
//

#include "CmGui.h"

bool CmGui::ToggleSwitch(const char *label, bool *v)
{
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    ImGui::Selectable(label, false);
    if (ImGui::IsItemClicked(ImGuiMouseButton_Left) || (!*v && ImGui::IsItemClicked(ImGuiMouseButton_Right)))
        *v = !*v;

    if(ImGui::IsItemHovered() || *v)
    {
        ImVec2 p = ImGui::GetItemRectMin();
        ImVec2 p_max = ImGui::GetItemRectMax();

        if(*v)
            draw_list->AddRectFilled(p, p_max, IM_COL32(150, 150, 150, 255), 0);
        else
            draw_list->AddRectFilled(p, p_max, IM_COL32(150, 150, 150, 100), 0);
    }
    return *v;
}
