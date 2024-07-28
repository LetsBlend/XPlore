//
// Created by Let'sBlend on 23/06/2024.
//

#include "XPloreManager.h"
#include "Util/Fonts.h"

#include <filesystem>
#include <sys/stat.h>

// For the RecycleBin
#include <cstdio>
#include <tchar.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <objbase.h>
#include <propkey.h>

static struct stat g_fileStatus;

bool XPloreManager::DoesPathExist(std::string &fullPath)
{
    if(!fullPath.ends_with('\\'))
        fullPath += '\\';

    if (fullPath == GetCurrentDisk() + "Recycle.Bin\\")
        return true;

    if(stat(fullPath.c_str(), &g_fileStatus) == 0)
        return true;
    return false;
}

void XPloreManager::AddNextNodes(Item &directory)
{
    directory.m_Children.clear();
    directory.m_IsChecked = true;
    if (directory.m_Parent == nullptr)
    {
        Item dir;
        dir.m_FullPath = directory.m_FullPath + "Recycle.Bin\\";
        dir.m_Name = "Recycle Bin";
        dir.m_IsFolder = true;
        dir.m_Parent = &directory;
        directory.m_Children.push_back(dir);

        dir.m_FullPath = directory.m_FullPath + "Users\\Let'sBlend\\Documents\\";
        dir.m_Name = "Documents";
        dir.m_IsFolder = true;
        dir.m_Parent = &directory;
        directory.m_Children.push_back(dir);

        dir.m_FullPath = directory.m_FullPath + "Users\\Let'sBlend\\Desktop\\";
        dir.m_Name = "Desktop";
        dir.m_IsFolder = true;
        dir.m_Parent = &directory;
        directory.m_Children.push_back(dir);

        dir.m_FullPath = directory.m_FullPath + "Users\\Let'sBlend\\Downloads\\";
        dir.m_Name = "Downloads";
        dir.m_IsFolder = true;
        dir.m_Parent = &directory;
        directory.m_Children.push_back(dir);

        dir.m_FullPath = directory.m_FullPath + "Users\\Let'sBlend\\Pictures\\";
        dir.m_Name = "Pictures";
        dir.m_IsFolder = true;
        dir.m_Parent = &directory;
        directory.m_Children.push_back(dir);

        dir.m_FullPath = directory.m_FullPath + "Users\\Let'sBlend\\Videos\\";
        dir.m_Name = "Videos";
        dir.m_IsFolder = true;
        dir.m_Parent = &directory;
        directory.m_Children.push_back(dir);

        dir.m_FullPath = directory.m_FullPath + "Users\\Let'sBlend\\Music\\";
        dir.m_Name = "Music";
        dir.m_IsFolder = true;
        dir.m_Parent = &directory;
        directory.m_Children.push_back(dir);
    }
    try
    {
        for (const auto &entry: std::filesystem::directory_iterator(directory.m_FullPath))
         {
             // Converting the path to const char * in the
             // subsequent lines
             std::filesystem::path outfilename = entry.path();
             std::string outfilename_str = outfilename.string();
             const char* path = outfilename_str.c_str();
             std::string fileName = entry.path().filename().string();

             if(entry.is_directory())
             {
                 Item dir;
                 dir.m_FullPath = path;
                 if (!dir.m_FullPath.ends_with('\\'))
                     dir.m_FullPath += '\\';
                 dir.m_Name = entry.path().filename().string();
                 dir.m_IsFolder = true;
                 dir.m_Parent = &directory;
                 directory.m_Children.push_back(dir);
             }
         }
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        Debug::Warn("Insufficient permission or", e.what());
    }
}

std::string XPloreManager::GetCurrentDisk()
{
    std::string lastSelectedDir = GetLastSelectedDirectory();
    int id = lastSelectedDir.find_first_of('\\');
    lastSelectedDir.erase(id + 1);
    return lastSelectedDir;
}

const SHCOLUMNID SCID_OriginalLocation =
        {PSGUID_DISPLACED, PID_DISPLACED_FROM};

const SHCOLUMNID SCID_DateDeleted =
        {PSGUID_DISPLACED, PID_DISPLACED_DATE};

void
XPloreManager::RecycleBinIterator(std::function<void(IShellFolder2 *pRecycleBinFolder, LPITEMIDLIST &pidlItem)> func)
{
    CoInitialize(NULL);

    IShellFolder *pDesktopFolder = nullptr;
    if (FAILED(SHGetDesktopFolder(&pDesktopFolder)))
    {
        CoUninitialize();
        return;
    }

    LPITEMIDLIST pidlRecycleBin = nullptr;
    if (FAILED(SHGetSpecialFolderLocation(NULL, CSIDL_BITBUCKET, &pidlRecycleBin)))
    {
        pDesktopFolder->Release();
        CoUninitialize();
        return;
    }

    IShellFolder2 *pRecycleBinFolder = nullptr;
    if (FAILED(pDesktopFolder->BindToObject(pidlRecycleBin, NULL, IID_IShellFolder, (void **) &pRecycleBinFolder)))
    {
        CoTaskMemFree(pidlRecycleBin);
        pDesktopFolder->Release();
        CoUninitialize();
        return;
    }

    IEnumIDList *pEnumIDList = nullptr;
    if (FAILED(pRecycleBinFolder->EnumObjects(NULL, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, &pEnumIDList)))
    {
        pRecycleBinFolder->Release();
        CoTaskMemFree(pidlRecycleBin);
        pDesktopFolder->Release();
        CoUninitialize();
        return;
    }

    LPITEMIDLIST pidlItem = nullptr;
    while (pEnumIDList->Next(1, &pidlItem, NULL) == S_OK)
    {
        func(pRecycleBinFolder, pidlItem);
    }

    pEnumIDList->Release();
    pRecycleBinFolder->Release();
    CoTaskMemFree(pidlRecycleBin);
    pDesktopFolder->Release();
    CoUninitialize();
}

bool XPloreManager::ItemSelected(const Item &item, IShellFolder2 *pRecycleBinFolder, LPITEMIDLIST &pidlItem)
{
    STRRET strRet;
    if (SUCCEEDED(pRecycleBinFolder->GetDisplayNameOf(pidlItem, SHGDN_NORMAL, &strRet)))
    {
        CHAR szDisplayName[MAX_PATH];
        if (StrRetToBufA(&strRet, pidlItem, szDisplayName, MAX_PATH) == S_OK)
        {
            std::string name = szDisplayName;
            int id = name.find_last_of('\\');
            name.erase(0, id + 1);

            // Get date last modified
            std::string dateLastModified;
            VARIANT ft;
            if (SUCCEEDED(pRecycleBinFolder->GetDetailsEx(pidlItem, &SCID_DateDeleted, &ft)))
            {
                DATE lastModified = ft.date;
                SYSTEMTIME sysTime;
                VariantTimeToSystemTime(lastModified, &sysTime); // Convert OLE Automation date to SYSTEMTIME
                char buffer[256];
                GetDateFormatA(LOCALE_USER_DEFAULT, 0, &sysTime, "yyyy-MM-dd", buffer,
                               sizeof(buffer)); // Example date format
                dateLastModified = buffer;
                dateLastModified.append(" ");
                GetTimeFormatA(LOCALE_USER_DEFAULT, 0, &sysTime, "HH:mm:ss", buffer, sizeof(buffer)); // Time format
                dateLastModified += buffer;
            }

            // Check if this is the file you want to restore
            if (name == item.m_Name && dateLastModified == item.m_DateLastModified)
                return true;
        }
    }
    return false;
}

std::vector<Item> XPloreManager::GetEntriesInDirectory(const std::string &folderPath)
{
    std::vector<Item> directories;
    if (folderPath == GetCurrentDisk() + "Recycle.Bin\\")
    {
        RecycleBinIterator([&](IShellFolder2 *pRecycleBinFolder, LPITEMIDLIST &pidlItem)
                           {
                               STRRET strRet;
                               if (SUCCEEDED(pRecycleBinFolder->GetDisplayNameOf(pidlItem, SHGDN_NORMAL, &strRet)))
                               {
                                   CHAR szDisplayName[MAX_PATH];
                                   if (StrRetToBufA(&strRet, pidlItem, szDisplayName, MAX_PATH) == S_OK)
                                   {
                                       std::string name = szDisplayName;
                                       int id = name.find_last_of('\\');
                                       name.erase(0, id + 1);

                                       Item dir;
                                       // Get date last modified
                                       VARIANT ft;
                                       if (SUCCEEDED(pRecycleBinFolder->GetDetailsEx(pidlItem, &SCID_DateDeleted, &ft)))
                                       {
                                           DATE lastModified = ft.date;
                                           SYSTEMTIME sysTime;
                                           VariantTimeToSystemTime(lastModified,
                                                                   &sysTime); // Convert OLE Automation date to SYSTEMTIME
                                           char buffer[256];
                                           GetDateFormatA(LOCALE_USER_DEFAULT, 0, &sysTime, "yyyy-MM-dd", buffer,
                                                          sizeof(buffer)); // Example date format
                                           dir.m_DateLastModified = buffer;
                                           dir.m_DateLastModified.append(" ");
                                           GetTimeFormatA(LOCALE_USER_DEFAULT, 0, &sysTime, "HH:mm:ss", buffer,
                                                          sizeof(buffer)); // Time format
                                           dir.m_DateLastModified += buffer;
                                       }

                                       // Determine if it's a folder
                                       IShellFolder *pParentFolder;
                                       LPCITEMIDLIST pidlChild = pidlItem;
                                       if (SUCCEEDED(
                                               SHBindToParent(pidlChild, IID_IShellFolder, (void **) &pParentFolder,
                                                              &pidlChild)))
                                       {
                                           ULONG attribs = SFGAO_FOLDER;
                                           if (SUCCEEDED(pParentFolder->GetAttributesOf(1, &pidlChild, &attribs)))
                                           {
                                               dir.m_IsFolder = (attribs & SFGAO_FOLDER) != 0;
                                           }
                                       }

                                       // Get file size
                                       dir.m_FileSize = -1;

                                       dir.m_FullPath = GetCurrentDisk() + "Recycle.Bin\\" + name;
                                       dir.m_Name = name;
                                       directories.push_back(dir);
                                   }
                               }

                               CoTaskMemFree(pidlItem);
                           });
        return directories;
    }

    try
    {
        for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
            std::filesystem::path outfilename = entry.path();
            std::string outfilename_str = outfilename.string();
            const char* path = outfilename_str.c_str();

            if(!entry.is_directory())
            {
                Item dir(path, entry.path().filename().string(), entry.is_directory());
                GetSizeAndDateModified(dir);

                directories.push_back(dir);
            }
        }
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        Debug::Warn("Insufficient permission or", e.what());
    }

    return directories;
}

std::string &XPloreManager::GetLastSelectedDirectory()
{
    return m_CurrentDirectoryPaths[m_CurrentDirectoryPaths.size() - 1];
}

void XPloreManager::ScaleFileSizes(uintmax_t fileSize, int &bytes, std::string &type)
{
    if(fileSize >= 1024 && fileSize < 1024 * 1024)
    {
        bytes = fileSize / 1024;
        type = "kb";
    }
    else if(fileSize >= 1024 * 1024 && fileSize < 1024 * 1024 * 1024)
    {
        bytes = fileSize / (1024 * 1024);
        type = "mb";
    }
    else if(fileSize >= 1024 * 1024 * 1024 && fileSize < (long long)1024 * 1024 * 1024 * 1024)
    {
        bytes = fileSize / (1024 * 1024 * 1024);
        type = "gb";
    }
    else if(fileSize >= (long long)1024 * 1024 * 1024 * 1024)
    {
        bytes = fileSize / ((long long)1024 * 1024 * 1024 * 1024);
        type = "tb";
    }
    if (bytes > 999)
        bytes = -1;
}

std::vector<std::string> XPloreManager::ConvertPathToNames(const std::string &fullPath)
{
    std::vector<std::string> names;
    std::string currentName;
    for(int i = 0; i < fullPath.size(); i++)
    {
        if(fullPath[i] == '\\')
        {
            names.push_back(currentName);
            currentName.clear();
            continue;
        }
        currentName += fullPath[i];

        if(i >= fullPath.size() - 1)
            names.push_back(currentName);
    }

    return names;
}

void XPloreManager::LaunchFile(const Item &file)
{
    int result = system(("\"" + file.m_FullPath + "\"").c_str());
    if(result == -1)
        Debug::Error("Failed to open file:", file.m_Name);
}

std::string XPloreManager::RenameItem(std::string &filePath, const std::string &newName)
{
    std::string source = filePath;
    if(source.ends_with('\\'))
        source.pop_back();

    int id = source.find_last_of('\\');
    source.erase(id + 1);

    source += newName;
    //std::rename(filePath.c_str(), source.c_str());

    SHFILEOPSTRUCTA fileOp = {0};
    fileOp.hwnd = NULL;
    fileOp.wFunc = FO_RENAME;
    filePath += '\0';
    source += '\0';
    fileOp.pFrom = filePath.c_str();
    fileOp.pTo = source.c_str();
    fileOp.fFlags = FOF_NOCONFIRMMKDIR | FOF_SILENT | FOF_ALLOWUNDO;
    int result = SHFileOperation(&fileOp);
    if(result != 0)
        Debug::Error("Failed to rename file with error code:", result);

    return source;
}

void XPloreManager::GetSizeAndDateModified(Item &dir)
{
    struct stat fileStatus;

    if(stat(dir.m_FullPath.c_str(), &fileStatus) >= 0)
    {
        time_t time = fileStatus.st_mtime;
        // Convert to tm structure
        std::tm* tm = std::localtime(&time);

        // Create a string stream to format the time
        std::stringstream ss;
        ss << std::put_time(tm, "%Y-%m-%d %H:%M:%S");

        dir.m_DateLastModified = ss.str();
        dir.m_FileSize = fileStatus.st_size;
    }
    else
    {
        dir.m_FileSize = -1;
        dir.m_DateLastModified = "???";
    }
}

void XPloreManager::AddItemsToClipBoard(const std::unordered_set<Item> &items, int clipBoardFlag)
{
    // Open the clipboard
    if (!OpenClipboard(nullptr))
        return;

    // Clear the clipboard
    if (!EmptyClipboard())
    {
        Debug::Error("Failed to empty clipboard");
        CloseClipboard();
        return;
    }

    // Allocate global memory for the item list
    size_t totalPathSize = 0;
    for (const Item &item: items)
        totalPathSize += (item.m_FullPath.size() + 1) * sizeof(char);

    size_t totalSize = sizeof(DROPFILES) + totalPathSize + sizeof(char);

    // Allocate global memory for the item list
    HGLOBAL hGlobal = GlobalAlloc(GHND, totalSize);
    if (!hGlobal)
    {
        Debug::Error("Failed to allocate global memory");
        CloseClipboard();
        return;
    }

    // Lock the global memory and fill it with the DROPFILES structure and item path
    DROPFILES *dropFiles = static_cast<DROPFILES *>(GlobalLock(hGlobal));
    if (!dropFiles)
    {
        Debug::Error("Failed to lock global memory");
        GlobalFree(hGlobal);
        CloseClipboard();
        return;
    }

    dropFiles->pFiles = sizeof(DROPFILES);
    dropFiles->pt.x = 0;
    dropFiles->pt.y = 0;
    dropFiles->fNC = FALSE;
    dropFiles->fWide = FALSE;

    char *pathBuffer = reinterpret_cast<char *>(dropFiles + 1);
    for (const Item &item: items)
    {
        strcpy(pathBuffer, item.m_FullPath.c_str());
        pathBuffer += item.m_FullPath.size() + 1;
    }
    *pathBuffer += '\0';

    // Set the file list data to the clipboard
    if (!SetClipboardData(CF_HDROP, hGlobal))
    {
        Debug::Error("Failed to set clipboard data");
        GlobalFree(hGlobal);
        CloseClipboard();
        return;
    }

    // Set the preferred drop effect to move
    HGLOBAL hGlobalEffect = GlobalAlloc(GHND, sizeof(DWORD));
    if (!hGlobalEffect)
    {
        Debug::Error("Failed to allocate global memory");
        CloseClipboard();
        return;
    }

    DWORD *pEffect = static_cast<DWORD *>(GlobalLock(hGlobalEffect));
    if (!pEffect)
    {
        Debug::Error("Failed to lock global memory");
        GlobalFree(hGlobalEffect);
        CloseClipboard();
        return;
    }

    *pEffect = clipBoardFlag;

    GlobalUnlock(hGlobal);

    // Set the clipboard data
    if (!SetClipboardData(RegisterClipboardFormat(CFSTR_PREFERREDDROPEFFECT), hGlobalEffect))
    {
        Debug::Error("Failed to set clipboard data");
        GlobalFree(hGlobal);
        CloseClipboard();
        return;
    }

    // Close the clipboard
    CloseClipboard();
}

std::vector<Source> XPloreManager::GetItemFromClipBoard()
{
    if (!OpenClipboard(nullptr))
        return {};

    HGLOBAL hGlobal = GetClipboardData(CF_HDROP);
    if (!hGlobal)
    {
        Debug::Error("Failed to get clipboard data");
        return {};
    }
    HDROP dropHandle = static_cast<HDROP>(GlobalLock(hGlobal));
    if (!dropHandle)
    {
        Debug::Error("Failed to lock global memory");
        return {};
    }

    UINT fileCount = DragQueryFileA(dropHandle, 0xFFFFFFFF, nullptr, 0);

    std::vector<Source> sources;
    for (int i = 0; i < fileCount; i++)
    {
        UINT pathLength = DragQueryFileA(dropHandle, i, nullptr, 0);
        if (pathLength > 0)
        {
            std::vector<char> path(pathLength + 1);
            DragQueryFileA(dropHandle, i, path.data(), pathLength + 1);

            sources.emplace_back(path.data(), path.data());

            Source &newSource = sources[sources.size() - 1];
            if (newSource.m_SourceName.ends_with('\\'))
                newSource.m_SourceName.pop_back();

            int id = newSource.m_SourceName.find_last_of('\\');
            newSource.m_SourceName.erase(0, id + 1);
        }
    }

    GlobalUnlock(hGlobal);
    CloseClipboard();

    return sources;
}

void XPloreManager::PasteFiles(const PasteOptions &pasteOptons, const std::string &dest)
{
    std::vector<Source> sources = GetItemFromClipBoard();

    for (Source &source: sources)
    {
        std::string destination = dest;
        if(!std::filesystem::is_directory(source.m_SourcePath))
            destination += source.m_SourceName;
        // pFrom and pTo must be double null terminated
        destination += '\0';
        source.m_SourcePath += '\0';

        Debug::Info(source.m_SourcePath);
        Debug::Info(destination);

        SHFILEOPSTRUCTA fileOp = {0};
        fileOp.hwnd = NULL; // Handle to the parent window of the progress dialog box
        fileOp.wFunc = pasteOptons & PasteOptions::Copy ?  FO_COPY : FO_MOVE; // Operation to be performed
        fileOp.pFrom = source.m_SourcePath.c_str(); // Source file path
        fileOp.pTo = destination.c_str(); // Destination file path
        fileOp.fFlags = FOF_NOCONFIRMMKDIR | FOF_SILENT | FOF_ALLOWUNDO; // Flags to control the operation

        int result = SHFileOperation(&fileOp);
        if (result != 0)
            Debug::Error("Failed to copy item with error code:", result);

        /* OLD CODE

        // Remove or Rename if file already exists
        int duplicates = 0;
        for(;;)
        {
            if(!std::filesystem::exists(destination))
                break;
            if(!(pasteOptons & PasteOptions::Duplicate))
                break;

            destination = dest + source.m_SourceName + "-Duplicate(" + toString(duplicates) + ")";
            if(!std::filesystem::exists(destination))
                break;

            duplicates++;
        }
        if (std::filesystem::is_directory(source.m_SourcePath))
        {
            if (pasteOptons & PasteOptions::Copy)
                system(("robocopy " + source.m_SourcePath + " " + destination + " /e").c_str());
            else
                system(("robocopy " + source.m_SourcePath + " " + destination + " /e /move").c_str());
        }
        else
        {
            system(("copy \"" + source.m_SourcePath + "\" \"" + destination + "\"").c_str());
            if (pasteOptons & PasteOptions::Cut)
                system(("del \"" + source.m_SourcePath + "\" /s /q").c_str());
        }
         */
    }
}

void XPloreManager::Delete(bool allowUndo, const Item &item, XPloreManager &xpManager)
{
    std::string deletePath = item.m_FullPath;
    if (deletePath.ends_with('\\'))
        deletePath.pop_back();
    deletePath += '\0';

    Debug::Info(deletePath);
    SHFILEOPSTRUCTA fileOp = {0};
    fileOp.hwnd = NULL;
    fileOp.wFunc = FO_DELETE;
    fileOp.pFrom = deletePath.c_str();
    fileOp.pTo = NULL;
    fileOp.fFlags = FOF_NOERRORUI | FOF_NOCONFIRMATION | FOF_SILENT;
    if(allowUndo)
        fileOp.fFlags |= FOF_ALLOWUNDO; // Allows for undo
    else
        fileOp.fFlags &= ~FOF_NOCONFIRMATION; // Shows the confirmation dialog
    int result = SHFileOperationA(&fileOp);

    if (result != 0)
        Debug::Error("Failed to delete file: " + toString(result));

    int id = deletePath.find_last_of('\\');
    deletePath.erase(id);
    xpManager.GetLastSelectedDirectory() = deletePath;
}

void XPloreManager::DeleteFromRecycleBin(const Item &item)
{
    RecycleBinIterator([&](IShellFolder2 *pRecycleBinFolder, LPITEMIDLIST &pidlItem)
                       {
                           if (ItemSelected(item, pRecycleBinFolder, pidlItem))
                           {
                               IContextMenu *pContextMenu = NULL;
                               if (SUCCEEDED(pRecycleBinFolder->GetUIObjectOf(NULL, 1, (LPCITEMIDLIST *) &pidlItem, IID_IContextMenu, NULL, (void **) &pContextMenu)))
                               {
                                   HMENU hMenu = CreatePopupMenu();
                                   if (hMenu)
                                   {
                                       if (SUCCEEDED(pContextMenu->QueryContextMenu(hMenu, 0, 1, 0x7FFF, CMF_DEFAULTONLY)))
                                       {
                                           CMINVOKECOMMANDINFOEX cmd = {0};
                                           cmd.cbSize = sizeof(CMINVOKECOMMANDINFOEX);
                                           cmd.fMask = CMIC_MASK_UNICODE;
                                           cmd.hwnd = NULL;
                                           cmd.lpVerb = "delete";
                                           cmd.lpVerbW = L"delete";
                                           cmd.nShow = SW_SHOWNORMAL;
                                           if (FAILED(pContextMenu->InvokeCommand((LPCMINVOKECOMMANDINFO) &cmd)))
                                               Debug::Error("Failed to restore file");
                                       }
                                       DestroyMenu(hMenu);
                                   }
                                   pContextMenu->Release();
                               }
                           }
                        /*
                           std::string selectedItem = ItemSelected(item, pRecycleBinFolder, pidlItem);

                           int id = selectedItem.find_last_of('\\');
                           selectedItem.erase(0, id + 1);
                           if (!selectedItem.empty())
                           {
                               int result = 0;
                               for (int i = 0; i < 5; i++)
                               {
                                   SHFILEOPSTRUCTA fileOp = {0};
                                   fileOp.hwnd = NULL;
                                   fileOp.wFunc = FO_DELETE;
                                   fileOp.pFrom = selectedItem.c_str();
                                   fileOp.pTo = NULL;
                                   fileOp.fFlags = FOF_NOERRORUI | FOF_NOCONFIRMATION | FOF_SILENT;
                                   result = SHFileOperationA(&fileOp);
                                   if (result == 0)
                                       break;
                               }
                               if (result != 0)
                                   Debug::Error("Failed to delete file: " + toString(result));
                           }
                           */
                       });
}

void XPloreManager::EmptyRecycleBin()
{
    HRESULT result = SHEmptyRecycleBin(NULL, NULL, 0);
    if(!result)
        Debug::Error("Failed to empty Recycle Bin with error code:", result);
}

void XPloreManager::Restore(const Item item)
{
    RecycleBinIterator([&](IShellFolder2 *pRecycleBinFolder, LPITEMIDLIST &pidlItem)
       {
           if (ItemSelected(item, pRecycleBinFolder, pidlItem))
           {
               IContextMenu *pContextMenu = NULL;
               if (SUCCEEDED(pRecycleBinFolder->GetUIObjectOf(NULL, 1, (LPCITEMIDLIST *) &pidlItem, IID_IContextMenu, NULL, (void **) &pContextMenu)))
               {
                   HMENU hMenu = CreatePopupMenu();
                   if (hMenu)
                   {
                       if (SUCCEEDED(pContextMenu->QueryContextMenu(hMenu, 0, 1, 0x7FFF, CMF_DEFAULTONLY)))
                       {
                           CMINVOKECOMMANDINFOEX cmd = {0};
                           cmd.cbSize = sizeof(CMINVOKECOMMANDINFOEX);
                           cmd.fMask = CMIC_MASK_UNICODE;
                           cmd.hwnd = NULL;
                           cmd.lpVerb = "undelete";
                           cmd.lpVerbW = L"undelete";
                           cmd.nShow = SW_SHOWNORMAL;
                           if (FAILED(pContextMenu->InvokeCommand((LPCMINVOKECOMMANDINFO) &cmd)))
                               Debug::Error("Failed to restore file");
                       }
                       DestroyMenu(hMenu);
                   }
                   pContextMenu->Release();
               }
           }
       });
}
