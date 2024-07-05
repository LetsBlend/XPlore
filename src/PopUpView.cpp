//
// Created by Let'sBlend on 27/06/2024.
//

#include "PopUpView.h"
#include "Gui/Gui.h"
#include "HirarchyView.h"
#include "DirectoryView.h"

#include <shlobj.h> // For SHFileOperation
#include <sys/stat.h>

void PopUpView::DisplayPopUp(XPloreManager &xpManager, HirarchyView &hirarchyView, DirectoryView &directoryView)
{
    if (m_IsOpen)
    {
        ImGui::OpenPopup("PopUp");
        m_IsOpen = false;
    }

    bool sameFile = false;
    if (ImGui::BeginPopup("PopUp"))
    {
        Debug::Info("Directories:", toString(m_Directories.size()));
        Debug::Info("Sources:", toString(m_Sources.size()));

        if (ImGui::Selectable("New"))
        {
            m_Sources.clear();
            for (const Directory &directory: m_Directories)
                m_Sources.push_back(Source(directory.m_FullPath, directory.m_FileName));

            m_CreateNew = true;
        }
        if (ImGui::Selectable("Open"))
        {
            for (const Directory &directory: m_Directories)
            {
                if (directory.m_IsDirectory)
                    continue;

                std::thread runThread(&XPloreManager::LaunchFile, xpManager, directory);
                runThread.detach();
            }
        }
        if (ImGui::Selectable("Open With"))
        {
            for (const Directory &directory: m_Directories)
            {
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
                if (directory.m_IsDirectory)
                {
                    std::string previousDirectory = xpManager.m_CurrentDirectoryPath;
                    previousDirectory.pop_back();
                    int id = previousDirectory.find_last_of('\\');
                    previousDirectory.erase(id + 1);
                    system(("cd /D " + previousDirectory + " && tar -a -c -f \"" + directory.m_FileName + ".zip\" \"" +
                            directory.m_FileName + "\"").c_str());
                } else
                {
                    system(("cd /D " + xpManager.m_CurrentDirectoryPath + " && tar -a -c -f \"" + directory.m_FileName +
                            ".zip\" \"" + directory.m_FileName + "\"").c_str());
                }
            }
            directoryView.m_Refresh = true;
            hirarchyView.m_Refresh = true;
        }
        if (ImGui::Selectable("Cut"))
        {
            m_Sources.clear();
            for (const Directory &directory: m_Directories)
                m_Sources.push_back(Source(directory.m_FullPath, directory.m_FileName));
            m_CopyOrCut = CopyOrCut::Cut;
        }
        if (ImGui::Selectable("Copy"))
        {
            m_Sources.clear();
            for (const Directory &directory: m_Directories)
                m_Sources.push_back(Source(directory.m_FullPath, directory.m_FileName));
            m_CopyOrCut = CopyOrCut::Copy;
        }
        if (ImGui::Selectable("Paste"))
        {
            for (int i = 0; i < m_Sources.size(); i++)
            {
                struct stat fileStatus;
                std::string newStr = xpManager.m_CurrentDirectoryPath;
                newStr += "\\" + m_Sources[i].m_SourceName;
                if (stat(newStr.c_str(), &fileStatus) >= 0)
                    sameFile = true;
            }

            if (!sameFile)
                PasteFiles(m_Sources, xpManager.m_CurrentDirectoryPath);

            directoryView.m_Refresh = true;
            hirarchyView.m_Refresh = true;
        }
        if (ImGui::Selectable("Rename"))
        {
            if(m_Directories.size() == 1)
            {
                m_Sources.clear();
                for (const Directory &directory: m_Directories)
                {
                    m_NewName = directory.m_FileName;
                    m_Sources.push_back(Source(directory.m_FullPath, directory.m_FileName));
                }
                m_Rename = true;
                directoryView.m_Refresh = true;
                hirarchyView.m_Refresh = true;
            }
        }
        if (ImGui::Selectable("Delete"))
        {
            for (const Directory &directory: m_Directories)
            {
                int result = 0;
                std::string deletePath = directory.m_FullPath;
                if (deletePath.ends_with('\\'))
                    deletePath.pop_back();
                deletePath += '\0';
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
                xpManager.m_CurrentDirectoryPath = deletePath;
            }
            directoryView.m_Refresh = true;
            hirarchyView.m_Refresh = true;
        }
        if (ImGui::Selectable("Permanently Delete"))
        {
            for (const Directory& directory: m_Directories)
            {
                std::string deletePath = directory.m_FullPath;
                if (deletePath.ends_with('\\'))
                    deletePath.pop_back();
                deletePath += '\0';
                if (directory.m_IsDirectory)
                {
                    if(directory.m_FullPath[directory.m_FullPath.size() - 4] == '.' && directory.m_FullPath[directory.m_FullPath.size() - 3] == 'z' &&
                    directory.m_FullPath[directory.m_FullPath.size() - 2] == 'i' && directory.m_FullPath[directory.m_FullPath.size() - 1] == 'p')
                        std::filesystem::remove(deletePath.c_str());
                    else
                        std::filesystem::remove_all(deletePath.c_str());
                }
                else
                    std::filesystem::remove(deletePath.c_str());

                int id = deletePath.find_last_of('\\');
                deletePath.erase(id);
                xpManager.m_CurrentDirectoryPath = deletePath;
            }
            directoryView.m_Refresh = true;
            hirarchyView.m_Refresh = true;
        }

        m_Directories.clear();
        ImGui::End();
    }
    if (sameFile)
    {
        ImGui::OpenPopup("##Replace");
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetMousePos().x - 280 - 50, ImGui::GetMousePos().y - 50 - 25));
    }

    ImGui::SetNextWindowSize(ImVec2(560, 130));
    if (ImGui::BeginPopup("##Replace"))
    {
        ImGui::SetCursorPosY(20);
        ImGui::Text("Are you sure you want to replace the file in destination?");
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 - 110);
        ImGui::SetCursorPosY(68);
        if (ImGui::Button("Yes", ImVec2(100, 30)))
        {
            PasteFiles(m_Sources, xpManager.m_CurrentDirectoryPath);
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();
        if (ImGui::Button("No", ImVec2(100, 30)))
        {
            ImGui::CloseCurrentPopup();
        }
        directoryView.m_Refresh = true;
        ImGui::End();
    }
}

void PopUpView::PasteFiles(std::vector<Source> &sources, const std::string &dest)
{
    for (int i = 0; i < sources.size(); i++)
    {
        std::string newStr = dest;
        newStr += sources[i].m_SourceName;
        if(sources[i].m_SourcePath.ends_with('\\'))
            sources[i].m_SourcePath.pop_back();

        try {
            std::filesystem::remove_all(newStr.c_str());
        }
        catch(std::filesystem::filesystem_error& e) {}
        if (m_CopyOrCut == CopyOrCut::Cut)
        {
            std::filesystem::copy(sources[i].m_SourcePath.c_str(), newStr.c_str());
            std::filesystem::remove_all(sources[i].m_SourcePath.c_str());
        }
        else
        {
            std::filesystem::copy(sources[i].m_SourcePath.c_str(), newStr.c_str());
        }
    }
}
