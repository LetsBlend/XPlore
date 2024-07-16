//
// Created by Let'sBlend on 28/06/2024.
//

#ifndef XPLORE_CUSTOMGUI_H
#define XPLORE_CUSTOMGUI_H

#include "Gui/Gui.h"

namespace ImGui
{
    bool ToggleSwitch(const char* label, bool* v);
    bool Spinner(const char* label, float radius, int thickness, const ImU32& color);
    bool BufferingBar(const char* label, float value,  const ImVec2& size_arg, const ImU32& bg_col, const ImU32& fg_col);
}


#endif //XPLORE_CUSTOMGUI_H
