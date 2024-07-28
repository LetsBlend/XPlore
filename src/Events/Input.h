//
// Created by Let'sBlend on 26/02/2024.
//

#ifndef CLOSINGIN_INPUT_H
#define CLOSINGIN_INPUT_H

#include "EventType.h"

struct KeyBoardState
{
    uint8_t key[256];
};

struct MouseState
{
    uint16_t pos_x;
    uint16_t pos_y;
    uint8_t button[3];
};

struct InputState
{
    KeyBoardState currentKeyState;
    KeyBoardState previousKeyState;
    MouseState currentMouseState;
    MouseState previousMouseState;
};

class Event;
class GLFWwindow;

class Input
{
public:
    // Check if the specified Key is Pressed
    static bool GetKeyDown(KeyCode key);
    // Check if the specified Key is Released
    static bool GetKeyUp(KeyCode key);
    // Check if the specified Key is Hold
    static bool GetKey(KeyCode key);

    // Check if the specified Mouse Button is Pressed
    static bool GetButtonDown(Button button);
    // Check if the specified Mouse Button is Released
    static bool GetButtonUp(Button button);
    // Check if the specified Mouse Button is Hold
    static bool GetButton(Button button);

    // Check if any Key was Pressed
    static bool AnyKeyDown();
    // Check if any Key was Released
    static bool AnyKeyUp();
    // Check if any Key was Hold
    static bool AnyKey();

    // Check if any Mouse Button was Pressed
    static bool AnyButtonDown();
    // Check if any Mouse Button was Released
    static bool AnyButtonUp();
    // Check if any Mouse Button was Hold
    static bool AnyButton();

protected:
    Input() = default;
    ~Input() = default;

    void Init(GLFWwindow* glfWwindow);

    void Update();

    inline static InputState inputState;
    inline static GLFWwindow* targetWindow;
};


#endif //CLOSINGIN_INPUT_H
