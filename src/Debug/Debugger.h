//
// Created by Let'sBlend on 19/03/2024.
//

#ifndef CLOSINGIN_DEBUGGER_H
#define CLOSINGIN_DEBUGGER_H

#include "Util/ConsoleColors.h"
#include "Util/StringConverter.h"
#include <source_location>
#include <iomanip>

#define SILENCE_WARN

struct Message
{
    template<typename Arg>
    Message(Arg m, std::source_location l = std::source_location::current()) : message(std::move(m)), location(std::move(l)) { }

    std::string message;
    std::source_location location;
};

class Debug
{
public:
#ifndef SILENCE_INFO
    template<typename... Args>
    inline static void Info(const Message& msg, const Args&... message)
    {
        std::string temp = msg.location.file_name();
        // Set color
        std::cout << ConsoleColors::WHITE;
        // Logs time
        std::cout <<'[' << time_in_HH_MM_SS_MMM() << "]";

        // Logs file and line
        std::cout << " [" << EraseTill(temp, "src") << " at " << msg.location.line() << "] ";

        // Logs Messages
        std::cout << "[INFO]: " << toString(msg.message) + ' ';
        (std::cout << ... << (toString(message) + ' '));
        // Clear color
        std::cout << ConsoleColors::RESET << '\n';
    }
#else
    template<typename... Args>
    inline static void Info(const Message& msg, const Args&... message)
    {
    }
#endif

#ifndef SILENCE_WARN
    template<typename... Args>
    inline static void Warn(const Message& msg, const Args&... message)
    {
        std::string temp = msg.location.file_name();
        // Set color
        std::cout << ConsoleColors::YELLOW_CUSTOM;
        // Logs time
        std::cout << '[' << time_in_HH_MM_SS_MMM() << "]";

        // Logs file and line
        std::cout << " [" << EraseTill(temp, "src") << " at " << msg.location.line() << "] ";

        // Logs Messages
        std::cout << "[WARN]: " << toString(msg.message) + ' ';
        (std::cout << ... << (toString(message) + ' '));
        // Clear color
        std::cout << ConsoleColors::RESET << '\n';
    }
#else
    template<typename... Args>
    inline static void Warn(const Message& msg, const Args&... message)
    {
    }
#endif

#ifndef SILENCE_ERROR
    template<typename... Args>
    inline static void Error(const Message& msg, const Args&... message)
    {
        std::string temp = msg.location.file_name();
        // Set color
        std::cout << ConsoleColors::RED_BRIGHT;
        // Logs time
        std::cout << '[' << time_in_HH_MM_SS_MMM() << "]";

        // Logs file and line
        std::cout << " [" << EraseTill(temp, "src") << " at " << msg.location.line() << "] ";

        // Logs Messages
        std::cout << "[ERROR]: " << toString(msg.message) + ' ';
        (std::cout << ... << (toString(message) + ' '));
        // Clear color
        std::cout << ConsoleColors::RESET << '\n';
    }
#else
    template<typename... Args>
    inline static void Error(const Message& msg, const Args&... message)
    {
    }
#endif

protected:
    Debug() = default;

private:
    static std::string time_in_HH_MM_SS_MMM()
    {
        using namespace std::chrono;
        auto now = system_clock::now();
        auto timer = system_clock::to_time_t(now);
        std::tm bt = *std::localtime(&timer);
        std::ostringstream oss; //TODO remove stream
        oss << std::put_time(&bt, "%H:%M:%S"); // HH:MM:SS
        return oss.str();
    }
};

#endif //CLOSINGIN_DEBUGGER_H
