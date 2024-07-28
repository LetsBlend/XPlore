#include "Application.h"
#include "Window/Window.h"
#include "Events/Input.h"
#include "Gui/Gui.h"

int main()
{
    Application app;
    Window window;

    Gui gui(window.GetWindow());

    app.ReadDrives();
    while(app.IsRunning())
    {
        app.Update(window, gui);
    }
}
