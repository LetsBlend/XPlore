//
// Created by Let'sBlend on 23/06/2024.
//

#ifndef XPLORE_XPLOREMANAGER_H
#define XPLORE_XPLOREMANAGER_H

struct Directory
{
    std::string m_FullPath;
    std::string m_Name;
    bool m_IsFolder = false;
    uintmax_t m_FileSize;
    std::string m_DateLastModified;
    std::vector<Directory> m_Children;
    Directory* parent;
    bool m_IsParentSelected = false;
    bool m_IsFileSelected = false;
    bool m_IsChecked = false;

    bool operator==(const Directory& other) const {
        if (m_Name == other.m_Name && m_FullPath == other.m_FullPath)
            return true;
        return false;
    };
};

namespace std {
    template<> struct hash<Directory>
    {
        size_t operator()(const Directory& p) const
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

    void AddNextNodes(Directory& directory);
    void ScaleFileSizes(uintmax_t fileSize, int& bytes, std::string& type);
    std::vector<std::string> ConvertPathToNames(const std::string& fullPath);

    std::vector<Directory> GetEntriesInDirectory(const std::string& folderPath);
    std::string& GetLastSelectedDirectory();

    void LaunchFile(const Directory& file);
    std::string RenameItem(const std::string& filePath, const std::string& newName);
    void PasteFiles(const PasteOptions& pasteOptons, std::vector<Source> &sources, const std::string &dest);
    void PermanentlyDelete(const Source &source);

    std::vector<std::string> m_CurrentDirectoryPaths{1, "C:\\"};
    std::vector<std::string> m_CurrentDirectoryPathNames{1};
    bool m_IsChangingHeaderPath = false;

private:
    void GetSizeAndDateModified(Directory& dir);
};

#endif //XPLORE_XPLOREMANAGER_H
