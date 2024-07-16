
//
// Created by Let'sBlend on 19/03/2024.
//

#ifndef CLOSINGIN_STRING_H
#define CLOSINGIN_STRING_H

#include <filesystem>
#include <chrono>

// Concept to check for container (has begin and end)
template<typename T>
concept Container = requires(T t) {
    t.begin();
    t.end();
};

// Concept to check for sequence container (is a container but does not have key_type)
template<typename T>
concept SequenceContainer = Container<T> && !requires { typename T::key_type; };

// Concept to check for associated container (is a container and has key_type)
template<typename T>
concept AssociatedContainer = Container<T> && requires { typename T::key_type; };


template<typename T>
concept to_stringable = std::is_arithmetic_v<T> || std::is_same_v<T, std::string>;

template<typename T>
concept IsEnum = std::is_enum_v<T>;

template<typename T>
concept IsClass = !Container<T> && std::is_class_v<T>;

// Handling primitives
template<to_stringable Arg>
static std::string toString(const Arg& value)
{
    return std::to_string(value);
}

// Handling strings
static std::string toString(const std::string& value)
{
    return value;
}

// Handling chars
static std::string toString(const char* value)
{
    return std::string(value);
}

// Handling enums
template<IsEnum Arg>
static std::string toString(const Arg& value)
{
    return std::to_string((std::underlying_type_t<Arg>(value)));
}

// Handling classes
template<IsClass Arg>
static std::string toString(const Arg& obj)
{
    return obj.toString();
}
/*
// Handle file system
static std::string to_String(const std::filesystem::file_time_type& ftime)
{
    std::time_t cftime = std::chrono::system_clock::to_time_t(
        std::chrono::file_clock::to_sys(ftime));
    std::string str = std::asctime(std::localtime(&cftime));
    str.pop_back(); // rm the trailing '\n' put by `asctime`
    return str;
}
*/

// Handling sequence containers
template<SequenceContainer Arg>
static std::string toString(const Arg& container)
{
    std::string result = "[";
    for(auto it = container.begin(); it != container.end();)
    {
        result += toString(*it); // Error here call of overloaded ... is ambigous
        if(++it != container.end()) result += ", ";
    }
    result += "]";
    return result;
}

// Handling associative containers
template<AssociatedContainer Arg>
static std::string toString(const Arg& container)
{
    std::string result = "{";
    for(auto it = container.begin(); it != container.end();)
    {
        result += "[" + toString(it->first) + ", " + toString(it->second) + "]";
        if(++it != container.end()) result += ", ";
    }
    result += "}";
    return result;
}

static std::string EraseTill(std::string& line, const std::string& word)
{
    auto it = line.find(word) + word.size() + 1;
    if(it == std::string::npos)
        return "";

    line.erase(0, it);
    return line;
}

#endif //CLOSINGIN_STRING_H
