#ifndef NOTIFIER_HPP
#define NOTIFIER_HPP

#include "user_choice.hpp"

UserChoice showNotification(
    std::chrono::system_clock::time_point startTime,
    const std::string& category,
    int limit
);

#endif

