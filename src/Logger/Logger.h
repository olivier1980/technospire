#pragma once
#include <iostream>
#include <vector>
#include <bitset>
#include <ctime>
#include <chrono>
#include "../Headers.h"

#define RESET "\033[0m"
#define GREEN "\033[32m"

enum LogLevel
{
    LEVEL_INFO = 0x1 << 0,
    LEVEL_WARNING = 0x1 << 1,
    LEVEL_ERROR = 0x1 << 2
};

#define LOG_LEVEL (IS_RELEASE ? LogLevel::LEVEL_ERROR : LogLevel::LEVEL_INFO)

enum LogType
{
    LOG_INFO = 0x1 << 0,
    LOG_WARNING = 0x1 << 1,
    LOG_ERROR = 0x1 << 2
};

struct LogEntry
{
    LogType type;
    std::string message;
};

class Logger {
public:
    static std::vector<LogEntry> messages; //one big vector for the class

    template <typename T>
    requires std::convertible_to<T, std::string>
    static void Log(T message);

    template <typename T>
    requires std::integral<T> || std::floating_point<T>
    static void Log(T message);

//    template <>
//    static void Log<char>(const char &_message);
    //static void Log(const std::string &_message);
    //static void Log(int _message);
    static void Err(const std::string &message);

    template<std::size_t N>
    static void Log(std::bitset<N> n);
    static std::string getDateString();
};

template <typename T>
requires std::convertible_to<T, std::string>
void Logger::Log(const T message) {
    if ((LOG_INFO & LOG_LEVEL) != LOG_LEVEL) {
        return;
    }
    auto s = getDateString();

    LogEntry logEntry;
    logEntry.type = LOG_INFO;
    logEntry.message =  "[" + s +  "] - " + message;

    messages.push_back(logEntry);
    std::cout << "[" << GREEN << s <<  RESET << "] - " << message << std::endl;
}

template <typename T>
requires std::integral<T> || std::floating_point<T>
void Logger::Log(const T message) {
    if ((LOG_INFO & LOG_LEVEL) != LOG_LEVEL) {
        return;
    }
    auto s = getDateString();

    LogEntry logEntry;
    logEntry.type = LOG_INFO;
    logEntry.message =  "[" + s +  "] - " + std::to_string(message);

    messages.push_back(logEntry);
    std::cout << "[" << GREEN << s <<  RESET << "] - " << message << std::endl;
}

template<std::size_t N>
void Logger::Log(std::bitset<N> n) {

    if ((LOG_INFO & LOG_LEVEL) != LOG_LEVEL) {
        return;
    }

    auto s = getDateString();

    LogEntry logEntry;
    logEntry.type = LOG_INFO;
    logEntry.message =  "[" + s +  "] - " + n.to_string();

    messages.push_back(logEntry);
    std::cout << "[" << GREEN << s <<  RESET << "] - " << n << std::endl;

}
