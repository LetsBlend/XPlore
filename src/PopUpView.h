//
// Created by Let'sBlend on 27/06/2024.
//

#ifndef XPLORE_POPUPVIEW_H
#define XPLORE_POPUPVIEW_H

#include "XPloreManager.h"

class DirectoryView;
class HirarchyView;

enum class CopyOrCut
{
    Copy,
    Cut
};

struct Source
{
    std::string m_SourcePath;
    std::string m_SourceName;
};

class PopUpView
{
public:
    void DisplayPopUp(XPloreManager& xpManager, HirarchyView& hirarchyView, DirectoryView& directoryView);
    void PasteFiles(std::vector<Source> &sources, const std::string& dest);

    bool m_IsOpen = false;
    bool m_Rename = false;
    bool m_CreateNew = false;
    bool m_NewFolder = false;
    std::string m_NewName;

    std::unordered_set<Directory> m_Directories;
    std::vector<Source> m_Sources;
    CopyOrCut m_CopyOrCut;
};


#endif //XPLORE_POPUPVIEW_H
