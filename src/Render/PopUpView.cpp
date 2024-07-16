//
// Created by Let'sBlend on 27/06/2024.
//

#include "PopUpView.h"
#include "Gui/Gui.h"
#include "Render/HirarchyView.h"
#include "Render/DirectoryView.h"

#include <shlobj.h> // For SHFileOperation
#include <sys/stat.h>

static std::vector<std::future<void>> g_AsyncOperation;
void PopUpView::DisplayPopUp(XPloreManager &xpManager, HirarchyView &hirarchyView, DirectoryView &directoryView)
{
    // Check if there is still an operation running
    int i = 0;
    for(const auto& asyncOp : g_AsyncOperation)
    {
        if(asyncOp.valid())
        {
            auto status = asyncOp.wait_for(0ms);
            if (status == std::future_status::ready)
                i++;
        }
    }
    if(i == g_AsyncOperation.size())
    {
        g_AsyncOperation.clear();
        directoryView.m_Processing = false;
    }

    // Begin Right Click PopUp
    if (m_IsOpen)
    {
        ImGui::OpenPopup("PopUp");
        m_IsOpen = false;
    }

    bool sameFile = false;
    bool permaDelete = false;
    if (ImGui::BeginPopup("PopUp"))
    {
        if (ImGui::Selectable("New"))
        {
            m_Sources.clear();
            for (const Directory &directory: m_Directories)
                m_Sources.push_back(Source(directory.m_FullPath, directory.m_Name));

            m_CreateNew = true;
        }
        if (ImGui::Selectable("Open"))
        {
            for (const Directory &directory: m_Directories)
            {
                if (directory.m_IsFolder)
                    continue;

                std::thread runThread(&XPloreManager::LaunchFile, xpManager, directory);
                runThread.detach();
            }
        }
        if (ImGui::Selectable("Open With"))
        {
            for (const Directory &directory: m_Directories)
            {
                if (directory.m_IsFolder)
                    continue;

                SHELLEXECUTEINFO shExInfo = {0};
                shExInfo.cbSize = sizeof(SHELLEXECUTEINFO);
                shExInfo.fMask = SEE_MASK_INVOKEIDLIST;
                shExInfo.hwnd = NULL;
                shExInfo.lpVerb = "openas";  // This is the key to open the "Open With" dialog
                shExInfo.lpFile = directory.m_FullPath.c_str();
                shExInfo.lpParameters = NULL;
                shExInfo.lpDirectory = NULL;
                shExInfo.nShow = SW_SHOW;
                shExInfo.hInstApp = NULL;
                ShellExecuteEx(&shExInfo);
            }
        }
        if (ImGui::Selectable("Compress to ZIP file"))
        {
            for (const Directory &directory: m_Directories)
            {
                g_AsyncOperation.push_back(std::async(std::launch::async, [&]() {
                    directoryView.m_Processing = true;
                    if (directory.m_IsFolder)
                    {
                        std::string previousDirectory = directory.m_FullPath;
                        previousDirectory.pop_back();
                        int id = previousDirectory.find_last_of('\\');
                        previousDirectory.erase(id + 1);
                        system(("cd /D " + previousDirectory + " && tar -a -c -f \"" + directory.m_Name + ".zip\" \"" +
                                directory.m_Name + "\"").c_str());
                    }
                    else
                    {
                        system(("cd /D " + xpManager.m_CurrentDirectoryPaths[0] + " && tar -a -c -f \"" + directory.m_Name +
                                ".zip\" \"" + directory.m_Name + "\"").c_str());
                    }
                    directoryView.m_Refresh = true;
                    hirarchyView.m_Refresh = true;
                }));
            }
        }
        if (ImGui::Selectable("Cut"))
        {
            m_Sources.clear();
            for (const Directory &directory: m_Directories)
                m_Sources.push_back(Source(directory.m_FullPath, directory.m_Name, directory.m_IsFolder));
            m_PasteOptions = PasteOptions::Cut;
        }
        if (ImGui::Selectable("Copy"))
        {
            m_Sources.clear();
            for (const Directory &directory: m_Directories)
                m_Sources.push_back(Source(directory.m_FullPath, directory.m_Name, directory.m_IsFolder));
            m_PasteOptions = PasteOptions::Copy;
        }
        if (ImGui::Selectable("Paste"))
        {
            for (Source& m_Source : m_Sources)
            {
                struct stat fileStatus;
                std::string newStr = xpManager.GetLastSelectedDirectory();
                newStr += "\\" + m_Source.m_SourceName;
                if (stat(newStr.c_str(), &fileStatus) >= 0)
                    sameFile = true;
            }

            if (!sameFile)
            {
                g_AsyncOperation.push_back(std::async(std::launch::async, [&]() {
                    directoryView.m_Processing = true;
                    xpManager.PasteFiles(m_PasteOptions, m_Sources, xpManager.GetLastSelectedDirectory());
                    directoryView.m_Refresh = true;
                    hirarchyView.m_Refresh = true;
                }));
            }
        }
        if (ImGui::Selectable("Rename"))
        {
            if(m_Directories.size() == 1)
            {
                m_Sources.clear();
                for (const Directory &directory: m_Directories)
                {
                    m_NewName = directory.m_Name;
                    m_Sources.push_back(Source(directory.m_FullPath, directory.m_Name));
                }
                m_Rename = true;
                directoryView.m_Refresh = true;
            }
        }
        if (ImGui::Selectable("Delete"))
        {
            directoryView.m_Processing = true;

            for (const Directory &directory: m_Directories)
            {
                g_AsyncOperation.push_back(std::async(std::launch::async, [&]() {
                    int result = 0;
                    std::string deletePath = directory.m_FullPath;
                    if (deletePath.ends_with('\\'))
                        deletePath.pop_back();
                    deletePath += '\0';
                    // Give it three tries because sometimes it randomly doesn't delete it the first time
                    for (int i = 0; i < 3; i++)
                    {
                        SHFILEOPSTRUCTA fileOp = {0};
                        fileOp.hwnd = NULL;
                        fileOp.wFunc = FO_DELETE;
                        fileOp.pFrom = deletePath.c_str();
                        fileOp.pTo = NULL;
                        fileOp.fFlags = FOF_ALLOWUNDO | FOF_NOERRORUI | FOF_NOCONFIRMATION | FOF_SILENT;
                        result = SHFileOperationA(&fileOp);
                        if (result == 0)
                            break;
                    }
                    if (result != 0)
                        MessageBoxA(NULL, ("Failed to delete file: " + toString(result)).c_str(), FALSE, 0);

                    int id = deletePath.find_last_of('\\');
                    deletePath.erase(id);
                    xpManager.GetLastSelectedDirectory() = deletePath;

                    directoryView.m_Refresh = true;
                    hirarchyView.m_Refresh = true;
                }));
            }
        }
        if (ImGui::Selectable("Permanently Delete"))
        {
            permaDelete = true;
            m_Sources.clear();

            for (const Directory &directory: m_Directories)
                m_Sources.push_back(Source(directory.m_FullPath, directory.m_Name, directory.m_IsFolder));

            directoryView.m_Refresh = true;
            hirarchyView.m_Refresh = true;
        }

        m_Directories.clear();
        ImGui::End();
    }

    // Replace File/Duplicate/None when copying
    if(sameFile)
    {
        ImGui::OpenPopup("##Replace");
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetMousePos().x - 280, ImGui::GetMousePos().y - 150));
    }
    ImGui::SetNextWindowSize(ImVec2(560, 250));
    if (ImGui::BeginPopup("##Replace"))
    {
        ImGui::SetCursorPosY(20);
        ImGui::PushFont(Gui::m_BoldFont);
        ImGui::Text("The destination already exists!");
        ImGui::PopFont();
        ImGui::SetCursorPosY(68);
        if (ImGui::Button("##Overwrite", ImVec2(ImGui::GetWindowWidth(), 50)))
        {
            g_AsyncOperation.push_back(std::async(std::launch::async, [&]() {
                directoryView.m_Processing = true;
                xpManager.PasteFiles(m_PasteOptions , m_Sources, xpManager.GetLastSelectedDirectory());
                directoryView.m_Refresh = true;
                hirarchyView.m_Refresh = true;
            }));
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        ImGui::SetCursorPosX(20);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
        ImGui::Text("Overwrite");
        if (ImGui::Button("##Duplicate", ImVec2(ImGui::GetWindowWidth(), 50)))
        {
            m_PasteOptions |= PasteOptions::Duplicate;
            g_AsyncOperation.push_back(std::async(std::launch::async, [&]() {
                directoryView.m_Processing = true;
                xpManager.PasteFiles(m_PasteOptions , m_Sources, xpManager.GetLastSelectedDirectory());
                directoryView.m_Refresh = true;
                hirarchyView.m_Refresh = true;
            }));
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        ImGui::SetCursorPosX(20);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
        ImGui::Text("Duplicate");
        if (ImGui::Button("##None", ImVec2(ImGui::GetWindowWidth(), 50)))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        ImGui::SetCursorPosX(20);
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10);
        ImGui::Text("None");

        ImGui::End();
    }

    // Permanently Delete
    if(permaDelete)
    {
        ImGui::OpenPopup("##PermaDelete");
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetMousePos().x - 280 - 50, ImGui::GetMousePos().y - 50 - 25));
    }
    ImGui::SetNextWindowSize(ImVec2(560, 130));
    if(ImGui::BeginPopup("##PermaDelete"))
    {
        ImGui::SetCursorPosY(20);
        ImGui::Text("Are you sure you want to replace the file in destination?");
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 - 110);
        ImGui::SetCursorPosY(68);
        if (ImGui::Button("Yes", ImVec2(100, 30)))
        {
            g_AsyncOperation.push_back(std::async(std::launch::async, [&]() {
                directoryView.m_Processing = true;
                for (const Source &source : m_Sources)
                    xpManager.PermanentlyDelete(source);
                directoryView.m_Refresh = true;
                hirarchyView.m_Refresh = true;
            }));

            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();
        if (ImGui::Button("No", ImVec2(100, 30)))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::End();
    }
}

