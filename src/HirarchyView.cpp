//
// Created by Let'sBlend on 27/06/2024.
//

#include "HirarchyView.h"
#include "XPloreManager.h"
#include "PopUpView.h"

#include "Gui/Gui.h"
#include "Util/Fonts.h"

static bool g_IsCtrlPressed = false;
static bool g_IsShiftPressed = false;
void HirarchyView::DisplayHirarchy(bool& itemClicked, Directory& directory, XPloreManager& xpManager, PopUpView& popUpView, int currentNameIndex, int startFlag)
{
    // Return if it's a file
    if(!directory.m_IsFolder)
        return;

    // Refresh Node Tree
    if(!directory.m_IsChecked || m_Refresh)
        xpManager.AddNextNodes(directory);

    // Set Flags
    int flag = ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | startFlag;
    startFlag = ImGuiTreeNodeFlags_None;

    // If Folder contains no Folders
    if (directory.m_Children.empty())
        flag |= ImGuiTreeNodeFlags_Leaf;
    
    // Check if we have selected Folder
    bool selected = false;
    for (const std::string &currentDirectory : xpManager.m_CurrentDirectoryPaths)
    {
        if (currentDirectory == directory.m_FullPath)
        {
            flag |= ImGuiTreeNodeFlags_Selected;
            selected = true;
            if(ImGui::IsWindowRectHovered() && !directory.m_IsParentSelected)
                popUpView.m_Directories.insert(directory);

            for(Directory& children : directory.m_Children)
                children.m_IsParentSelected = true;
        }
    }

    // Handle Selecting child while Parent is already selected
    if(directory.m_IsParentSelected)
    {
        for(Directory& children : directory.m_Children)
            children.m_IsParentSelected = true;
    }

    // Update Hierarchy to HeaderPath
    if(xpManager.m_CurrentDirectoryPathNames[currentNameIndex] == directory.m_Name && xpManager.m_IsChangingHeaderPath)
    {
        currentNameIndex++;
        ImGui::SetNextItemOpen(true);
    }

    // Rename Folder
    std::string source;
    if(!popUpView.m_Sources.empty())
        source = popUpView.m_Sources[0].m_SourceName;
    if ((source == directory.m_Name) && popUpView.m_Rename && xpManager.m_CurrentDirectoryPaths.size() == 1)
    {
        ImGui::SetKeyboardFocusHere();
        ImGui::SetItemDefaultFocus();
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

        if(ImGui::InputText("##label", &popUpView.m_NewName, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue))
        {
            if(ImGui::IsKeyDown(ImGuiKey_Enter) || ImGui::IsKeyDown(ImGuiKey_Escape))
            {
                xpManager.m_CurrentDirectoryPaths[0] = xpManager.RenameItem(popUpView.m_Sources[0].m_SourcePath,
                                                                            popUpView.m_NewName);
                popUpView.m_Rename = false;
                xpManager.AddNextNodes(*directory.parent);
            }
        }
        if(!ImGui::IsItemHovered() && ImGui::IsAnyMouseDown())
        {
            xpManager.m_CurrentDirectoryPaths[0] = xpManager.RenameItem(popUpView.m_Sources[0].m_SourcePath,
                                                                        popUpView.m_NewName);
            popUpView.m_Rename = false;
            xpManager.AddNextNodes(*directory.parent);
        }

        ImGui::PopItemWidth();
        ImGui::SameLine();
        flag = ImGuiTreeNodeFlags_None;
    }
    // Normal/Ctrl/Shift selecting files
    if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl))
        g_IsCtrlPressed = true;
    else if (ImGui::IsKeyReleased(ImGuiKey_LeftCtrl) || ImGui::IsKeyReleased(ImGuiKey_RightCtrl))
        g_IsCtrlPressed = false;

    if (ImGui::IsKeyDown(ImGuiKey_LeftShift) || ImGui::IsKeyDown(ImGuiKey_RightShift))
        g_IsShiftPressed = true;
    else if (ImGui::IsKeyReleased(ImGuiKey_LeftShift) || ImGui::IsKeyReleased(ImGuiKey_RightShift))
        g_IsShiftPressed = false;

    bool opened = ImGui::TreeNodeEx(("##" + directory.m_Name).c_str(), flag | ImGuiTreeNodeFlags_AllowItemOverlap);
    if (ImGui::IsItemClicked())
    {
        // Clear if not shift or ctrl pressing
        if (!g_IsShiftPressed && !g_IsCtrlPressed)
            xpManager.m_CurrentDirectoryPaths.clear();

        // Shift Click
        if (g_IsShiftPressed && directory.parent != nullptr)
        {
            bool load = false;
            int id = 0;
            int prevID = 0;
            int currentID = 0;
            for (const Directory& drNode : directory.parent->m_Children)
            {
                if(drNode.m_FullPath == xpManager.m_CurrentDirectoryPaths[xpManager.m_CurrentDirectoryPaths.size() - 1])
                    prevID = id;

                if(drNode.m_FullPath == directory.m_FullPath)
                    currentID = id;
                id++;
            }

            for(int i = (prevID < currentID ? prevID : currentID); i < (currentID > prevID ? currentID : prevID); i++)
            {
                xpManager.m_CurrentDirectoryPaths.emplace_back(directory.parent->m_Children[i].m_FullPath);
            }
        }

        // Ctrl Select Folder
        if(std::count(xpManager.m_CurrentDirectoryPaths.begin(), xpManager.m_CurrentDirectoryPaths.end(), directory.m_FullPath) > 0 && g_IsCtrlPressed)
            xpManager.m_CurrentDirectoryPaths.erase(std::remove(xpManager.m_CurrentDirectoryPaths.begin(), xpManager.m_CurrentDirectoryPaths.end(), directory.m_FullPath), xpManager.m_CurrentDirectoryPaths.end());
        else
            xpManager.m_CurrentDirectoryPaths.push_back(directory.m_FullPath);
    }

    // Right click Menu
    if(ImGui::IsItemClicked(ImGuiMouseButton_Right))
    {
        if (!g_IsShiftPressed && !g_IsCtrlPressed && !selected)
            xpManager.m_CurrentDirectoryPaths.clear();
        if(std::count(xpManager.m_CurrentDirectoryPaths.begin(), xpManager.m_CurrentDirectoryPaths.end(), directory.m_FullPath) == 0)
            xpManager.m_CurrentDirectoryPaths.push_back(directory.m_FullPath);
        popUpView.m_IsOpen = true;
        popUpView.m_NewFolder = true;
        itemClicked = true;
    }

    // Display Folder Names
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(255.0f / 255.0f, 217.0f / 255.0f, 112.0f / 255.0f, 1.0f));
    ImGui::SameLine();
    ImGui::Text(ICON_FA_FOLDER);
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::Text(directory.m_Name.c_str());

    // Create Folder
    if(!popUpView.m_Sources.empty())
        source = popUpView.m_Sources[0].m_SourcePath;
    if (popUpView.m_CreateNew && source == directory.m_FullPath && popUpView.m_NewFolder && xpManager.m_CurrentDirectoryPaths.size() == 1)
    {
        ImGui::SetKeyboardFocusHere();
        ImGui::SetItemDefaultFocus();
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

        if(ImGui::InputText("##label", &popUpView.m_NewName, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue))
        {
            if(ImGui::IsKeyDown(ImGuiKey_Enter) || ImGui::IsKeyDown(ImGuiKey_Escape))
            {
                try
                {
                    std::filesystem::create_directory(xpManager.m_CurrentDirectoryPaths[0] + popUpView.m_NewName);
                    xpManager.AddNextNodes(*directory.parent);
                }
                catch(const std::filesystem::filesystem_error& e)
                {
                    Debug::Error(e.what());
                }
                popUpView.m_CreateNew = false;
                popUpView.m_NewFolder = false;
            }
        }
        if(!ImGui::IsItemHovered() && ImGui::IsAnyMouseDown())
        {
            try
            {
                std::filesystem::create_directory(xpManager.m_CurrentDirectoryPaths[0] + popUpView.m_NewName);
                xpManager.AddNextNodes(*directory.parent);
            }
            catch(const std::filesystem::filesystem_error& e)
            {
                Debug::Error(e.what());
            }
            popUpView.m_CreateNew = false;
            popUpView.m_NewFolder = false;
        }
        ImGui::PopItemWidth();
    }

    // Next Node
    if (opened)
    {
        for (Directory& drNode : directory.m_Children)
            DisplayHirarchy(itemClicked, drNode, xpManager, popUpView, currentNameIndex, startFlag);

        ImGui::TreePop();
    }
    directory.m_IsParentSelected = false;
}
