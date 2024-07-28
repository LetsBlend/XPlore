//
// Created by Let'sBlend on 23/06/2024.
//

#ifndef XPLORE_XPLOREMANAGER_H
#define XPLORE_XPLOREMANAGER_H

#include <shobjidl.h>

struct Item
{
    std::string m_FullPath;
    std::string m_Name;
    bool m_IsFolder = false;
    uintmax_t m_FileSize;
    std::string m_DateLastModified;
    std::vector<Item> m_Children;
    Item* m_Parent;
    bool m_IsParentSelected = false;
    bool m_IsFileSelected = false;
    bool m_IsChecked = false;

    bool operator==(const Item& other) const {
        if (m_Name == other.m_Name && m_FullPath == other.m_FullPath)
            return true;
        return false;
    };
};

namespace std {
    template<> struct hash<Item>
    {
        size_t operator()(const Item& p) const
        {
            return (hash<string>()(p.m_Name)) ^ (hash<string>()(p.m_FullPath));
        }
    };
}

struct Source
{
    std::string m_SourcePath;
    std::string m_SourceName;
    bool m_IsFolder;
    std::string m_SourceDateLastModified;
};

enum PasteOptions
{
    None = 0,
    Copy = 1 << 0,
    Cut = 1 << 1,
    Duplicate = 1 << 2
};
DEFINE_ENUM_FLAG_OPERATORS(PasteOptions)

class XPloreManager
{
public:
    XPloreManager() = default;
    ~XPloreManager() = default;

    bool DoesPathExist(std::string& fullPath);

    void AddNextNodes(Item& directory);
    void ScaleFileSizes(uintmax_t fileSize, int& bytes, std::string& type);
    std::vector<std::string> ConvertPathToNames(const std::string& fullPath);

    std::string GetCurrentDisk();
    void RecycleBinIterator(std::function<void(IShellFolder2* pRecycleBinFolder, LPITEMIDLIST& pidlItem)> update, std::function<void(IShellFolder2 *pRecycleBinFolder)> postUpdate);
    bool ItemSelected(const Item& item, IShellFolder2* pRecycleBinFolder, LPITEMIDLIST& pidlItem);
    std::vector<Item> GetEntriesInDirectory(const std::string& folderPath);
    std::string& GetLastSelectedDirectory();

    void LaunchFile(Item file);
    std::string RenameItem(std::string& filePath, const std::string& newName);
    void AddItemsToClipBoard(const std::unordered_set<Item>& items, int clipBoardFlag);
    std::vector<Source> GetItemFromClipBoard();
    void PasteFiles(PasteOptions pasteOptons, std::string dest);
    void Restore(const std::unordered_set<Item>& item);

    void Delete(bool allowUndo, const std::unordered_set<Item>& items, XPloreManager& xpManager);
    void EmptyRecycleBin();
    void DeleteFromRecycleBin(const std::unordered_set<Item> &items);

    void TriggerUndo();

    std::vector<std::string> m_CurrentDirectoryPaths{1, "C:\\"};
    std::vector<std::string> m_CurrentDirectoryPathNames{1};
    bool m_IsChangingHeaderPath = false;

private:
    void GetSizeAndDateModified(Item& dir);
};

#endif //XPLORE_XPLOREMANAGER_H
