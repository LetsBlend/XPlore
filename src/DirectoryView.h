//
// Created by Let'sBlend on 27/06/2024.
//

#ifndef XPLORE_DIRECTORYVIEW_H
#define XPLORE_DIRECTORYVIEW_H

class Directory;
class XPloreManager;
class PopUpView;

class DirectoryView
{
public:
    void DisplayLoadingCircle();
    void DisplayLoadingBar();
    void DisplayFilePath(XPloreManager& xpManager);
    void DisplayDirectory(XPloreManager& xpManager, PopUpView& popUpView);

    std::vector<Directory> m_Directories;
    bool m_Refresh;
    std::atomic<bool> m_Processing;
};


#endif //XPLORE_DIRECTORYVIEW_H
