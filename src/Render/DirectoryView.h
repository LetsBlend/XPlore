//
// Created by Let'sBlend on 27/06/2024.
//

#ifndef XPLORE_DIRECTORYVIEW_H
#define XPLORE_DIRECTORYVIEW_H

class Item;
class XPloreManager;
class PopUpView;

class DirectoryView
{
public:
    void DisplayLoadingCircle();
    void DisplayLoadingBar();
    void DisplayFilePath(XPloreManager& value);
    void DisplayDirectory(XPloreManager& xpManager, PopUpView& popUpView);

    std::vector<Item> m_Items;
    bool m_Refresh;
    std::atomic<bool> m_Processing;
};


#endif //XPLORE_DIRECTORYVIEW_H
