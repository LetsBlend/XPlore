//
// Created by Let'sBlend on 23/06/2024.
//

#include "XPloreManager.h"
#include <filesystem>
#include <sys/stat.h>

static struct stat g_fileStatus;

bool XPloreManager::DoesPathExist(std::string &fullPath)
{
    if(!fullPath.ends_with('\\'))
        fullPath += '\\';

    if(stat(fullPath.c_str(), &g_fileStatus) == 0)
        return true;
    return false;
}

void XPloreManager::AddNextNodes(Directory& directory)
{
    directory.m_Children.clear();
    directory.m_IsChecked = true;
    try
    {
         for (const auto& entry : std::filesystem::directory_iterator(directory.m_FullPath))
         {
             // Converting the path to const char * in the
             // subsequent lines
             std::filesystem::path outfilename = entry.path();
             std::string outfilename_str = outfilename.string();
             const char* path = outfilename_str.c_str();
             std::string fileName = entry.path().filename().string();

             if(entry.is_directory())
             {
                 Directory dir;
                 dir.m_FullPath = path;
                 if (!dir.m_FullPath.ends_with('\\'))
                     dir.m_FullPath += '\\';
                 dir.m_FileName = entry.path().filename().string();
                 dir.m_IsDirectory = true;
                 directory.m_Children.push_back(dir);
             }
         }
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        Debug::Warn("Insufficient permission or", e.what());
    }
}

std::vector<Directory> XPloreManager::GetEntriesInDirectory(const std::string &folderPath)
{
    std::vector<Directory> directories;
    try
    {
        for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
            std::filesystem::path outfilename = entry.path();
            std::string outfilename_str = outfilename.string();
            const char* path = outfilename_str.c_str();

            if(!entry.is_directory())
            {
                Directory dir(path, entry.path().filename().string(), entry.is_directory());
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

void XPloreManager::LaunchFile(const Directory& file)
{
    int result = system(("\"" + file.m_FullPath + "\"").c_str());
    if(result == -1)
        Debug::Error("Failed to open file:", file.m_FileName);
}

std::string XPloreManager::RenameFile(const std::string &filePath, const std::string &newName)
{
    std::string source = filePath;
    if(source.ends_with('\\'))
        source.pop_back();

    int id = source.find_last_of('\\');
    source.erase(id + 1);

    source += newName;
    std::rename(filePath.c_str(), source.c_str());
    return source;
}

void XPloreManager::GetSizeAndDateModified(Directory& dir)
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
