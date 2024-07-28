//
// Created by Let'sBlend on 27/06/2024.
//

#include "PopUpView.h"
#include "Gui/Gui.h"
#include "Render/HirarchyView.h"
#include "Render/DirectoryView.h"

#include <shlobj.h> // For SHFileOperation
#include <sys/stat.h>

void PopUpView::AddToOperationQueue(const Item &item)
{
    m_Items.insert(item);
}

void PopUpView::MarkOperationArea(OperationArea opArea)
{
    m_OpArea = opArea;
}
bool PopUpView::IsCurrentOperationArea(OperationArea opArea)
{
    return m_OpArea == opArea;
}

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
    if (m_OpenUp)
    {
        ImGui::OpenPopup("PopUp");
        m_OpenUp = false;
    }

    bool sameFile = false;
    //bool permaDelete = false;
    m_IsOpen = false;
    if (ImGui::BeginPopup("PopUp"))
    {
        m_IsOpen = true;
        Debug::Info(std::to_string(m_Items.size()));
        //Debug::Info(std::to_string(m_Sources.size()));

        if (ImGui::Selectable("New"))
        {
            m_Sources.clear();
            for (const Item &item: m_Items)
                m_Sources.push_back(Source(item.m_FullPath, item.m_Name));

            m_CreateNew = true;
        }
        if (ImGui::Selectable("Open"))
        {
            for (const Item &item: m_Items)
            {
                if (item.m_IsFolder)
                    continue;

                std::thread runThread(&XPloreManager::LaunchFile, xpManager, item);
                runThread.detach();
            }
        }
        if (ImGui::Selectable("Open With"))
        {
            for (const Item &item: m_Items)
            {
                if (item.m_IsFolder)
                    continue;

                SHELLEXECUTEINFO shExInfo = {0};
                shExInfo.cbSize = sizeof(SHELLEXECUTEINFO);
                shExInfo.fMask = SEE_MASK_INVOKEIDLIST;
                shExInfo.hwnd = NULL;
                shExInfo.lpVerb = "openas";  // This is the key to open the "Open With" dialog
                shExInfo.lpFile = item.m_FullPath.c_str();
                shExInfo.lpParameters = NULL;
                shExInfo.lpDirectory = NULL;
                shExInfo.nShow = SW_SHOW;
                shExInfo.hInstApp = NULL;
                ShellExecuteEx(&shExInfo);
            }
        }
        if (ImGui::Selectable("Compress to ZIP file"))
        {
            for (const Item &item: m_Items)
            {
                g_AsyncOperation.push_back(std::async(std::launch::async, [&](Item item)
                {
                    directoryView.m_Processing = true;
                    if (item.m_IsFolder)
                    {
                        std::string previousDirectory = item.m_FullPath;
                        previousDirectory.pop_back();
                        int id = previousDirectory.find_last_of('\\');
                        previousDirectory.erase(id + 1);
                        system(("cd /D " + previousDirectory + " && tar -a -c -f \"" + item.m_Name + ".zip\" \"" +
                                item.m_Name + "\"").c_str());
                    }
                    else
                    {
                        system(("cd /D " + xpManager.m_CurrentDirectoryPaths[0] + " && tar -a -c -f \"" + item.m_Name +
                                ".zip\" \"" + item.m_Name + "\"").c_str());
                    }
                    directoryView.m_Refresh = true;
                    hirarchyView.m_Refresh = true;
                }, item));
            }
        }
        if (ImGui::Selectable("Cut"))
        {
            m_Sources.clear();
            xpManager.AddItemsToClipBoard(m_Items, DROPEFFECT_MOVE);
            m_PasteOptions = PasteOptions::Cut;
        }
        if (ImGui::Selectable("Copy"))
        {
            m_Sources.clear();
            xpManager.AddItemsToClipBoard(m_Items, DROPEFFECT_COPY);
            m_PasteOptions = PasteOptions::Copy;
        }
        if (ImGui::Selectable("Paste"))
        {
            g_AsyncOperation.push_back(std::async(std::launch::async, [&]
            {
                directoryView.m_Processing = true;
                xpManager.PasteFiles(m_PasteOptions, xpManager.GetLastSelectedDirectory());
                directoryView.m_Refresh = true;
                hirarchyView.m_Refresh = true;
            }));
        }
        if (ImGui::Selectable("Rename"))
        {
            if(m_Items.size() == 1)
            {
                m_Sources.clear();
                for (const Item &item : m_Items)
                {
                    m_NewName = item.m_Name;
                    m_Sources.push_back(Source(item.m_FullPath, item.m_Name));
                }
                m_Rename = true;
                directoryView.m_Refresh = true;
            }
        }
        if (ImGui::Selectable("Restore"))
        {
            directoryView.m_Processing = true;

            g_AsyncOperation.push_back(std::async(std::launch::async, [&](std::unordered_set<Item> items)
            {
                xpManager.Restore(items);
                directoryView.m_Refresh = true;
            }, m_Items));
        }
        if(xpManager.GetLastSelectedDirectory() == xpManager.GetCurrentDisk() + "Recycle.Bin\\")
        {
            if (ImGui::Selectable("Empty"))
            {
                g_AsyncOperation.push_back(std::async(std::launch::async, [&] {
                    xpManager.EmptyRecycleBin();
                }));
                directoryView.m_Refresh = true;
            }
        }
        else
        {
            if (ImGui::Selectable("Delete"))
            {
                directoryView.m_Processing = true;

                g_AsyncOperation.push_back(std::async(std::launch::async, [&](std::unordered_set<Item> items)
                {
                    xpManager.Delete(true, items, xpManager);
                    directoryView.m_Refresh = true;
                    hirarchyView.m_Refresh = true;
                }, m_Items));
            }
        }
        if (ImGui::Selectable("Permanently Delete"))
        {
            directoryView.m_Processing = true;

            g_AsyncOperation.push_back(std::async(std::launch::async, [&](std::unordered_set<Item> items)
            {
                if (xpManager.GetLastSelectedDirectory() == xpManager.GetCurrentDisk() + "Recycle.Bin\\")
                        xpManager.DeleteFromRecycleBin(items);
                else
                    xpManager.Delete(false, items, xpManager);
                directoryView.m_Refresh = true;
                hirarchyView.m_Refresh = true;
            }, m_Items));
        }

        m_Items.clear();
        ImGui::End();
    }

    /* OLD CODE
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
                xpManager.PasteFiles(m_PasteOptions, xpManager.GetLastSelectedDirectory());
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
                xpManager.PasteFiles(m_PasteOptions, xpManager.GetLastSelectedDirectory());
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
        ImGui::Text("Are you sure you want to permanently delete this item?");
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() / 2 - 110);
        ImGui::SetCursorPosY(68);
        if (ImGui::Button("Yes", ImVec2(100, 30)))
        {
            g_AsyncOperation.push_back(std::async(std::launch::async, [&]() {
                directoryView.m_Processing = true;
                for (const Source &source : m_Sources)
                    xpManager.PermanentlyDelete(source, xpManager);
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
     */
}

