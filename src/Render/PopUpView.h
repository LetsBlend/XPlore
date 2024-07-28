//
// Created by Let'sBlend on 27/06/2024.
//

#ifndef XPLORE_POPUPVIEW_H
#define XPLORE_POPUPVIEW_H

#include "XPloreManager.h"

class DirectoryView;
class HirarchyView;

enum class OperationArea
{
    HirarchyView,
    DirectoryView
};

class PopUpView
{
public:
    void AddToOperationQueue(const Item& item);
    void MarkOperationArea(OperationArea opArea);
    bool IsCurrentOperationArea(OperationArea opArea);

    void DisplayPopUp(XPloreManager& items, HirarchyView& hirarchyView, DirectoryView& directoryView);

    bool m_OpenUp = false;
    bool m_IsOpen = false;
    bool m_Rename = false;
    bool m_CreateNew = false;
    bool m_NewFolder = false;
    std::string m_NewName;

    std::unordered_set<Item> m_Items;
    std::vector<Source> m_Sources;
    PasteOptions m_PasteOptions;
    OperationArea m_OpArea;
};


#endif //XPLORE_POPUPVIEW_H
