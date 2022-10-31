#include "Logger.h"
#include <ctime>
#include <chrono>
#include <vector>
#include <bitset>

std::vector<LogEntry> Logger::messages;

std::string Logger::getDateString() {
    //std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    time_t curr_time;
    tm *curr_tm;
    char date_string[100];

    time(&curr_time);
    curr_tm = localtime(&curr_time);

    strftime(date_string, 50, "%d-%m-%Y %H:%M:%S", curr_tm);
    return date_string;
}


void Logger::Err(const std::string &message) {
    auto s = getDateString();
    LogEntry logEntry;
    logEntry.type = LOG_ERROR;
    logEntry.message =  "[" +s +  "] - " + message;

    messages.push_back(logEntry);
    std::cerr << "[\x1B[91m" << s  <<  "\033[0m] - " << message << std::endl;
}

