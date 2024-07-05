//
// Created by Let'sBlend on 23/06/2024.
//

#ifndef XPLORE_XPLOREMANAGER_H
#define XPLORE_XPLOREMANAGER_H

struct Directory
{
    std::string m_FullPath;
    std::string m_FileName;
    bool m_IsDirectory;
    uintmax_t m_FileSize;
    std::string m_DateLastModified;
    std::vector<Directory> m_Children;
    bool m_IsSelected;
    bool m_IsChecked;

    bool operator==(const Directory& other) const {
        if (m_FileName == other.m_FileName && m_FullPath == other.m_FullPath)
            return true;
        return false;
    };
};

namespace std {
    template<> struct hash<Directory>
    {
        size_t operator()(const Directory& p) const
        {
            return (hash<string>()(p.m_FileName)) ^ (hash<string>()(p.m_FullPath));
        }
    };
}

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

    void LaunchFile(const Directory& file);
    std::string RenameFile(const std::string& filePath, const std::string& newName);

    std::string m_CurrentDirectoryPath = "C:\\";
    std::vector<std::string> m_CurrentDirectoryPathNames{1};
    bool m_IsChangingHeaderPath = false;

private:
    void GetSizeAndDateModified(Directory& dir);
};

#endif //XPLORE_XPLOREMANAGER_H
