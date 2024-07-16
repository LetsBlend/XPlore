//
// Created by Let'sBlend on 22/06/2024.
//

#include "Window.h"
#include "GLFW/glfw3.h"
#include "glad/glad.h"

Window::Window()
{
    if (!glfwInit())
        return;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_Window = glfwCreateWindow(1920, 1080, "XPlore", nullptr, nullptr);
    if (!m_Window)
    {
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(m_Window);
    glfwSwapInterval(0);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
}

Window::~Window()
{
    glfwDestroyWindow(m_Window);
    glfwTerminate();
}

void Window::PollEvents(bool& running)
{
    glfwPollEvents();

    if(glfwWindowShouldClose(m_Window))
        running = false;
}

void Window::SwapBuffers()
{
    glfwSwapBuffers(m_Window);
}

void Window::Clear()
{
    int display_w, display_h;
    glfwGetFramebufferSize(GetWindow(), &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(.6, .6, .6, 1);
    glClear(GL_COLOR_BUFFER_BIT);
}

GLFWwindow *Window::GetWindow()
{
    return m_Window;
}

void Window::SetContext(GLFWwindow* context)
{
    glfwMakeContextCurrent(context);
}

GLFWwindow *Window::GetContext()
{
    return glfwGetCurrentContext();
}
