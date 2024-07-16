//
// Created by Let'sBlend on 27/06/2024.
//

#ifndef XPLORE_HIRARCHYVIEW_H
#define XPLORE_HIRARCHYVIEW_H

class Directory;
class XPloreManager;
class PopUpView;

class HirarchyView
{
public:
    void DisplayHirarchy(bool& itemClicked, Directory& directory, XPloreManager& xpManager, PopUpView& popUpView, int currentNameIndex, int startFlag);

    bool m_Refresh;
};


#endif //XPLORE_HIRARCHYVIEW_H
