//
// Created by Let'sBlend on 26/02/2024.
//

#include "Input.h"
#include "glfw/glfw3.h"

void Input::Init(GLFWwindow* glfWwindow)
{
    targetWindow = glfWwindow;
}

void Input::Update()
{
    for(int i = 0; i < 256; i++)
    {
        inputState.previousKeyState.key[i] = glfwGetKey(targetWindow, i);
    }

    inputState.previousMouseState.button[0] = glfwGetMouseButton(targetWindow, 0);
    inputState.previousMouseState.button[1] = glfwGetMouseButton(targetWindow, 1);
    inputState.previousMouseState.button[2] = glfwGetMouseButton(targetWindow, 2);
}

bool Input::GetKeyDown(KeyCode key)
{
    int keyCode = glfwGetKey(targetWindow, (int)key);
    inputState.currentKeyState.key[(int)key] = keyCode;

    if(keyCode != inputState.previousKeyState.key[(int)key] && keyCode == 1)
        return true;

    return false;
}

bool Input::GetKeyUp(KeyCode key)
{
    int keyCode = glfwGetKey(targetWindow, (int)key);
    inputState.currentKeyState.key[(int)key] = keyCode;

    if(keyCode != inputState.previousKeyState.key[(int)key] && keyCode == 0)
        return true;

    return false;
}
bool Input::GetKey(KeyCode key)
{
    if(glfwGetKey(targetWindow, (int)key))
        return true;

    return false;
}
bool Input::GetButtonDown(Button mouseButton)
{
    int keyCode = glfwGetMouseButton(targetWindow, (int)mouseButton);
    inputState.currentMouseState.button[(int)mouseButton] = keyCode;

    if(keyCode != inputState.previousMouseState.button[(int)mouseButton] && keyCode == 1)
        return true;

    return false;
}
bool Input::GetButtonUp(Button mouseButton)
{
    int keyCode = glfwGetMouseButton(targetWindow, (int)mouseButton);
    inputState.currentMouseState.button[(int)mouseButton] = keyCode;

    if(keyCode != inputState.previousMouseState.button[(int)mouseButton] && keyCode == 0)
        return true;

    return false;
}

bool Input::GetButton(Button mouseButton)
{
    if(glfwGetMouseButton(targetWindow, (int)mouseButton))
        return true;

    return false;
}
bool Input::AnyKeyDown()
{
    for (int i = 0; i < 256; i++)
    {
        int keyCode = glfwGetKey(targetWindow, i);
        if (keyCode > inputState.previousKeyState.key[i])
            return true;
    }

    return false;
}
bool Input::AnyKeyUp()
{
    for (int i = 0; i < 256; i++)
    {
        int keyCode = glfwGetKey(targetWindow, i);
        if (keyCode < inputState.previousKeyState.key[i])
            return true;
    }

    return false;
}

bool Input::AnyKey()
{
    for (int i = 0; i < 256; i++)
    {
        int keyCode = glfwGetKey(targetWindow, i);
        if (keyCode > 0)
            return true;
    }

    return false;
}

bool Input::AnyButtonDown()
{
    for (int i = 0; i < 3; i++)
    {
        int keyCode = glfwGetMouseButton(targetWindow, i);
        if (keyCode > inputState.previousMouseState.button[i])
            return true;
    }

    return false;
}

bool Input::AnyButtonUp()
{
    for (int i = 0; i < 3; i++)
    {
        int keyCode = glfwGetMouseButton(targetWindow, i);
        if (keyCode < inputState.previousMouseState.button[i])
            return true;
    }

    return false;
}

bool Input::AnyButton()
{
    for (int i = 0; i < 3; i++)
    {
        int keyCode = glfwGetMouseButton(targetWindow, i);
        if (keyCode > 0)
            return true;
    }

    return false;
}
