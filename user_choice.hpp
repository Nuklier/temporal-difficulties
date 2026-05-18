// common.hpp - user choice used in both TImeTracker and notifier_l.hpp
#ifndef COMMON_HPP
#define COMMON_HPP

#include <chrono>
#include <string>

enum class UserChoice {
    STOP_ACTIVITY,
    REMIND_LATER
};

// Предварительные объявления функций
extern void sendStatistics(std::string category, int minutes);
extern UserChoice showNotification(std::chrono::system_clock::time_point startTime, 
                                    const std::string& category, 
                                    int limit);

#endif // COMMON_HPP