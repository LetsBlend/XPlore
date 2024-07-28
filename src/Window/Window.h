//
// Created by Let'sBlend on 22/06/2024.
//

#ifndef XPLORE_WINDOW_H
#define XPLORE_WINDOW_H

class GLFWwindow;

class Window : Input
{
public:
    Window();
    ~Window();

    GLFWwindow* GetWindow();

    void SetContext(GLFWwindow* context);
    GLFWwindow* GetContext();

    void PollEvents(bool& running);
    void SwapBuffers();
    void Clear();

private:
    GLFWwindow* m_Window;
};


#endif //XPLORE_WINDOW_H
