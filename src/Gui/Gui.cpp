//
// Created by Let'sBlend on 22/06/2024.
//

#include "Gui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Window/Window.h"

#include "Util/Fonts.h"

namespace ImGui
{
    bool IsWindowRectHovered()
    {
        if (ImGui::GetMousePos().x >= ImGui::GetWindowPos().x &&
            ImGui::GetMousePos().x <= (ImGui::GetWindowPos().x + ImGui::GetWindowSize().x) &&
            ImGui::GetMousePos().y >= ImGui::GetWindowPos().y &&
            ImGui::GetMousePos().y <= (ImGui::GetWindowPos().y + ImGui::GetWindowSize().y))
            return true;

        return false;
    }
}


Gui::Gui(GLFWwindow* window)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    ImGui::StyleColorsDark();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    // Resize
    ImGui::GetStyle().ScaleAllSizes(1.5f);
    io.Fonts->Clear();
    m_RegularFont = io.Fonts->AddFontFromFileTTF("Assets/Roboto-Regular.ttf", 26);

    static const ImWchar  iconRanges[]{0xf000, 0xf3ff, 0};
    ImFontConfig iconsConfig;
    iconsConfig.MergeMode = true;
    iconsConfig.PixelSnapH = true;
    iconsConfig.OversampleH = 3;
    iconsConfig.OversampleV = 3;
    io.Fonts->AddFontFromMemoryCompressedTTF(Fonts::font_awesome_data, Fonts::font_awesome_size, 19, &iconsConfig, iconRanges);
    io.Fonts->Build();
    m_BoldFont = io.Fonts->AddFontFromFileTTF("Assets/Roboto-Bold.ttf", 26);
}

Gui::~Gui()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Gui::StartFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Gui::Render()
{
    ImGui::Render();
}

void Gui::EndFrame(Window& window)
{
    ImGuiIO io = ImGui::GetIO();

    // Rendering
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Update and Render additional Platform Windows
    // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
    //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = window.GetContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        window.SetContext(backup_current_context);
    }
}
