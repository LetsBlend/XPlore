//
// Created by Let'sBlend on 27/06/2024.
//

#include "DirectoryView.h"
#include "XPloreManager.h"
#include "PopUpView.h"

#include "Gui/CustomGui.h"
#include "Util/Fonts.h"

void DirectoryView::DisplayLoadingCircle()
{
    if(!m_Processing)
        return;
    const ImU32 col = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
    ImGui::Spinner("##Spinny", 15, 6, col);
    ImGui::SameLine();
}

void DirectoryView::DisplayLoadingBar()
{
    const ImU32 col = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
    const ImU32 bg = ImGui::GetColorU32(ImGuiCol_Button);
    ImGui::BufferingBar("##BuffyBar", 0.7f, ImVec2(ImGui::GetWindowWidth(), 6), bg, col);
}

void DirectoryView::DisplayFilePath(XPloreManager& xpManager)
{
    ImGui::PushItemWidth(ImGui::GetWindowWidth());
    xpManager.m_IsChangingHeaderPath = false;
    if(ImGui::InputText("##Path", &xpManager.GetLastSelectedDirectory()) && xpManager.DoesPathExist(xpManager.GetLastSelectedDirectory()))
    {
        xpManager.m_CurrentDirectoryPathNames = xpManager.ConvertPathToNames(xpManager.GetLastSelectedDirectory());
        xpManager.m_IsChangingHeaderPath = true;
    }
    ImGui::PopItemWidth();
}

static bool g_IsCtrlPressed = false;
static bool g_IsShiftPressed = false;
static int g_PreviousSelectedDirId = 0;
void DirectoryView::DisplayDirectory(XPloreManager& xpManager, PopUpView& popUpView)
{
    // Calculate Formating
    int longestText = 200;
    for(const Directory& directory : m_Directories)
    {
        int length = (int)ImGui::CalcTextSize(directory.m_Name.c_str()).x;
        if(length >= longestText)
            longestText = length;
    }

    // Window Headers
    ImGui::PushFont(Gui::m_BoldFont);
    ImGui::SetCursorPosX(22);
    ImGui::Text("Name");
    ImGui::SameLine();
    ImGui::SetCursorPosX(0);
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (float)longestText + 98);
    ImGui::Text("Date Modified");
    ImGui::SameLine();
    ImGui::SetCursorPosX(0);
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (float)longestText + ImGui::CalcTextSize("2023-10-19 16:52:14").x + 136);
    ImGui::Text("Size");
    ImGui::SetCursorPosY(125);
    ImGui::PopFont();

    // Create New File
    if(popUpView.m_CreateNew && !popUpView.m_NewFolder && xpManager.m_CurrentDirectoryPaths.size() == 1)
    {
        ImGui::SetKeyboardFocusHere();
        ImGui::SetItemDefaultFocus();
        ImGui::PushItemWidth(ImGui::GetWindowWidth());

        if(ImGui::InputText("##label", &popUpView.m_NewName, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue))
        {
            if(ImGui::IsKeyDown(ImGuiKey_Enter) || ImGui::IsKeyDown(ImGuiKey_Escape))
            {
                std::ofstream outFile(xpManager.GetLastSelectedDirectory() + popUpView.m_NewName);
                outFile.close();
                popUpView.m_CreateNew = false;
            }
        }
        if(!ImGui::IsItemHovered() && ImGui::IsAnyMouseDown())
        {
            std::ofstream outFile(xpManager.GetLastSelectedDirectory() + popUpView.m_NewName);
            outFile.close();
            popUpView.m_CreateNew = false;
        }
        ImGui::PopItemWidth();
        m_Refresh = true;
    }

    // Handle Right Click Menu when not hovering Item
    if(ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
    {
        popUpView.m_IsOpen = true;
        popUpView.m_NewFolder = false;
    }

    // Check for files in directory
    std::string source;
    if(!m_Directories.empty())
    {
        source = m_Directories[0].m_FullPath;
        int id = source.find_last_of('\\');
        source.erase(id + 1);
    }

    if(source != xpManager.GetLastSelectedDirectory())
        m_Refresh = true;

    if(m_Refresh)
    {
        m_Directories = xpManager.GetEntriesInDirectory(xpManager.GetLastSelectedDirectory());
        m_Refresh = false;
    }

    // Check if files or path exists
    if(!xpManager.DoesPathExist(xpManager.GetLastSelectedDirectory()))
    {
        ImGui::Text("The current path does not exist");
        return;
    }
    else if(m_Directories.empty() && !popUpView.m_CreateNew)
    {
        ImGui::Text("There are no files inside the directory or the directory is compressed(.zip)");
        return;
    }

    int id = 0;
    for(Directory& directory : m_Directories)
    {
        if(directory.m_IsFolder)
        {
            // "DEPRECATED" MAYBE IN THE FUTURE FOLDERS WILL BE DISPLAYED IN THE DIRECTORY WINDOW AS WELL
            // First part - Red
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(255.0f / 255.0f, 217.0f / 255.0f, 112.0f / 255.0f, 1.0f));
            ImGui::Text(ICON_FA_FOLDER);
            ImGui::PopStyleColor();

            ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
            ImGui::Text(directory.m_Name.c_str());
            ImGui::PopStyleColor();
        }
        else
        {
            // File Renaming
            if(!popUpView.m_Sources.empty())
            {
                if((popUpView.m_Sources[0].m_SourceName == directory.m_Name) && popUpView.m_Rename)
                {
                    ImGui::SetKeyboardFocusHere();
                    ImGui::SetItemDefaultFocus();
                    ImGui::PushItemWidth(ImGui::GetWindowWidth());

                    if(ImGui::InputText("##label", &popUpView.m_NewName, ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue))
                    {
                        if(ImGui::IsKeyDown(ImGuiKey_Enter) || ImGui::IsKeyDown(ImGuiKey_Escape))
                        {
                            xpManager.RenameItem(popUpView.m_Sources[0].m_SourcePath, popUpView.m_NewName);
                            popUpView.m_Rename = false;
                        }
                    }
                    if(!ImGui::IsItemHovered() && ImGui::IsAnyMouseDown())
                    {
                        xpManager.RenameItem(popUpView.m_Sources[0].m_SourcePath, popUpView.m_NewName);
                        popUpView.m_Rename = false;
                    }
                    ImGui::PopItemWidth();
                    m_Refresh = true;
                    continue;
                }
            }

            // Normal/Ctrl/Shift selecting files
            if(ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl))
                g_IsCtrlPressed = true;
            else if(ImGui::IsKeyReleased(ImGuiKey_LeftCtrl) || ImGui::IsKeyReleased(ImGuiKey_RightCtrl))
                g_IsCtrlPressed = false;

            if(ImGui::IsKeyDown(ImGuiKey_LeftShift) || ImGui::IsKeyDown(ImGuiKey_RightShift))
                g_IsShiftPressed = true;
            else if(ImGui::IsKeyReleased(ImGuiKey_LeftShift) || ImGui::IsKeyReleased(ImGuiKey_RightShift))
                g_IsShiftPressed = false;

            if(ImGui::ToggleSwitch(("##" + directory.m_Name).c_str(), &directory.m_IsFileSelected) && ImGui::IsWindowRectHovered())
            {
                popUpView.m_Directories.insert(directory);

                if(g_IsShiftPressed && ImGui::IsItemClicked())
                {
                    int subId = 0;
                    for(Directory& dir : m_Directories)
                    {
                        if(g_PreviousSelectedDirId < id)
                        {
                            if(subId >= g_PreviousSelectedDirId && subId <= id)
                                dir.m_IsFileSelected = true;
                        }
                        else
                        {
                            if(subId >= id && subId <= g_PreviousSelectedDirId)
                                dir.m_IsFileSelected = true;
                        }
                        subId++;
                    }
                }

                if(!g_IsCtrlPressed && !g_IsShiftPressed && ImGui::IsMouseDown(ImGuiMouseButton_Left) && ImGui::IsWindowFocused())
                {
                    for(Directory& dir : m_Directories)
                    {
                        if(dir.m_FullPath != directory.m_FullPath || !ImGui::IsItemHovered())
                            dir.m_IsFileSelected = false;
                    }
                }
                g_PreviousSelectedDirId = id;
            }

            // Handle Right Click Manu
            if(ImGui::IsItemClicked(ImGuiMouseButton_Right))
            {
                popUpView.m_Directories.insert(directory);
                popUpView.m_IsOpen = true;
                popUpView.m_NewFolder = false;
            }

            // Open Files
            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                std::thread fileThread(&XPloreManager::LaunchFile, xpManager, directory);
                fileThread.detach();
            }

            // Handling Name on the File with Color
            ImGui::SameLine();
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(.9f, .9f, .9f, .9f));
            ImGui::Text(ICON_FA_FILE);
            ImGui::PopStyleColor();

            ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
            ImGui::Text(directory.m_Name.c_str());
            ImGui::PopStyleColor();

            // Handle Date
            ImGui::SameLine();
            ImGui::SetCursorPosX(0);
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (float)longestText + 100);
            ImGui::Text(directory.m_DateLastModified.c_str());

            // Handle Size
            ImGui::SameLine();
            int bytes = directory.m_FileSize;
            std::string type = "b";
            xpManager.ScaleFileSizes(directory.m_FileSize, bytes, type);

            ImGui::SetCursorPosX(0);
            if(bytes <= -1)
            {
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (float)longestText + ImGui::CalcTextSize("2023-10-19 16:52:14").x - ImGui::CalcTextSize("???").x + 200);
                ImGui::Text("???");
            }
            else
            {
                ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (float)longestText + ImGui::CalcTextSize("2023-10-19 16:52:14").x - ImGui::CalcTextSize((toString(bytes) + type).c_str()).x + 200);
                ImGui::Text((toString(bytes) + type).c_str());
            }
        }

        id++;
    }
}
