#include "Application.h"
#include "Window/Window.h"
#include "Gui/Gui.h"

int main()
{
    Application app;
    Window window;
    Gui gui(window.GetWindow());

    while(app.IsRunning())
    {
        app.Update(window, gui);
    }
}
