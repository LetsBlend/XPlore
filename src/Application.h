//
// Created by Let'sBlend on 22/06/2024.
//

#ifndef XPLORE_APPLICATION_H
#define XPLORE_APPLICATION_H

class Window;
class Gui;

class Application
{
public:
    Application() = default;
    ~Application() = default;

    bool IsRunning();
    void Update(Window& window, Gui& gui);

private:
    bool m_Running = true;
};


#endif //XPLORE_APPLICATION_H
