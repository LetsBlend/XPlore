//
// Created by Let'sBlend on 27/06/2024.
//

#include "HirarchyView.h"
#include "XPloreManager.h"
#include "PopUpView.h"

#include "Gui/Gui.h"
#include "Util/Fonts.h"

void HirarchyView::DisplayHirarchy(bool& itemClicked, Directory& directory, XPloreManager& xpManager, PopUpView& popUpView, int currentNameIndex, int startFlag)
{
    // Return if it's a file
    if(!directory.m_IsDirectory)
        return;

    // Refresh Node Tree
    if(!directory.m_IsChecked || ImGui::IsKeyDown(ImGuiKey_F5) || m_Refresh)
        xpManager.AddNextNodes(directory);

    // Set Flags
    int flag = ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | startFlag;
    startFlag = ImGuiTreeNodeFlags_None;

    // If Folder contains no Folders
    if(!directory.m_IsChecked)
        flag |= ImGuiTreeNodeFlags_Leaf;
    
    // Check if we have selected Folder
    if(xpManager.m_CurrentDirectoryPath == directory.m_FullPath)
    {
        flag |= ImGuiTreeNodeFlags_Selected;
        if (ImGui::GetMousePos().x >= ImGui::GetWindowPos().x && ImGui::GetMousePos().x <= (ImGui::GetWindowPos().x + ImGui::GetWindowSize().x) &&
                ImGui::GetMousePos().y >= ImGui::GetWindowPos().y && ImGui::GetMousePos().y <= (ImGui::GetWindowPos().y + ImGui::GetWindowSize().y)) {
            popUpView.m_Directories.insert(directory);
        }
    }

    // Update Hierarchy to HeaderPath
    if(xpManager.m_CurrentDirectoryPathNames[currentNameIndex] == directory.m_FileName && xpManager.m_IsChangingHeaderPath)
    {
        currentNameIndex++;
        ImGui::SetNextItemOpen(true);
    }

    // Rename Folder
    std::string source;
    if(!popUpView.m_Sources.empty())
        source = popUpView.m_Sources[0].m_SourceName;
    if((source == directory.m_FileName) && popUpView.m_Rename && ImGui::IsWindowHovered())
    {
        ImGui::SetKeyboardFocusHere();
        ImGui::SetItemDefaultFocus();
        ImGui::PushItemWidth(ImGui::GetWindowWidth());

        if(ImGui::InputText("##label", &popUpView.m_NewName, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue))
        {
            if(ImGui::IsKeyDown(ImGuiKey_Enter) || ImGui::IsKeyDown(ImGuiKey_Escape))
            {
                xpManager.m_CurrentDirectoryPath = xpManager.RenameFile(popUpView.m_Sources[0].m_SourcePath, popUpView.m_NewName);
                popUpView.m_Rename = false;
            }
        }
        if(!ImGui::IsItemHovered() && ImGui::IsAnyMouseDown())
        {
            xpManager.m_CurrentDirectoryPath = xpManager.RenameFile(popUpView.m_Sources[0].m_SourcePath, popUpView.m_NewName);
            popUpView.m_Rename = false;
        }
        ImGui::PopItemWidth();
        m_Refresh = true;
    }
    else
    {
        // Display and Update TreeNode if clicked
        bool opened = ImGui::TreeNodeEx(("##" + directory.m_FileName).c_str(), flag);
        if (ImGui::IsItemClicked() && !itemClicked)
        {
            xpManager.m_CurrentDirectoryPath = directory.m_FullPath;
            itemClicked = true;
            if (ImGui::GetMousePos().x >= ImGui::GetWindowPos().x && ImGui::GetMousePos().x <= (ImGui::GetWindowPos().x + ImGui::GetWindowSize().x) &&
                ImGui::GetMousePos().y >= ImGui::GetWindowPos().y && ImGui::GetMousePos().y <= (ImGui::GetWindowPos().y + ImGui::GetWindowSize().y))
                popUpView.m_Directories.insert(directory);
        }

        // Right click Menu
        if(ImGui::IsMouseDown(ImGuiMouseButton_Right) && ImGui::IsItemHovered())
        {
            popUpView.m_IsOpen = true;
            popUpView.m_NewFolder = true;
            xpManager.m_CurrentDirectoryPath = directory.m_FullPath;
            itemClicked = true;
        }

        // Display Folder Names
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(255.0f / 255.0f, 217.0f / 255.0f, 112.0f / 255.0f, 1.0f));
        ImGui::SameLine();
        ImGui::Text(ICON_FA_FOLDER);
        ImGui::PopStyleColor();
        ImGui::SameLine();
        ImGui::Text(directory.m_FileName.c_str());

        // Create Folder
        if(!popUpView.m_Sources.empty())
            source = popUpView.m_Sources[0].m_SourcePath;
        if(popUpView.m_CreateNew && source == directory.m_FullPath && popUpView.m_NewFolder)
        {
            ImGui::SetKeyboardFocusHere();
            ImGui::SetItemDefaultFocus();
            ImGui::PushItemWidth(ImGui::GetWindowWidth());

            if(ImGui::InputText("##label", &popUpView.m_NewName, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue))
            {
                if(ImGui::IsKeyDown(ImGuiKey_Enter) || ImGui::IsKeyDown(ImGuiKey_Escape))
                {
                    try
                    {
                        std::filesystem::create_directory(xpManager.m_CurrentDirectoryPath + popUpView.m_NewName);
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
                    std::filesystem::create_directory(xpManager.m_CurrentDirectoryPath + popUpView.m_NewName);
                }
                catch(const std::filesystem::filesystem_error& e)
                {
                    Debug::Error(e.what());
                }
                popUpView.m_CreateNew = false;
                popUpView.m_NewFolder = false;
            }
            ImGui::PopItemWidth();
            m_Refresh = true;
        }

        // Next Node
        if (opened)
        {
            for (Directory& drNode : directory.m_Children)
                DisplayHirarchy(itemClicked, drNode, xpManager, popUpView, currentNameIndex, startFlag);

            ImGui::TreePop();
        }
    }
}
