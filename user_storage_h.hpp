#include "user_storage.hpp"
#include <iostream>
#include <chrono>
#include <cstring>
#include <string>
#include <sstream>

std::string UserStorage::getCurrentDate() {
    time_t now = time(nullptr);
    char buf[11];
    strftime(buf, sizeof(buf), "%Y-%m-%d", localtime(&now));
    return std::string(buf);
}

void UserStorage::updateTodayCache() {
    std::string current = getCurrentDate();
    if (today_date != current) {
        today_usage.clear();
        today_date = current;
    }
}

UserStorage::UserStorage(const std::string& name) : user_name(name) {
    today_date = getCurrentDate();
}

void UserStorage::setLimit(const std::string& category, int minutes) {
    limits[category] = minutes;
}

int UserStorage::getLimit(const std::string& category) {
    if (limits.find(category) != limits.end()) {
        return limits[category];
    }
    return 0;
}

void UserStorage::addSession(const std::string& category, int minutes) {
    // Создаём сессию
    ActivitySession session;
    session.category = category;
    session.end_time = time(nullptr);
    session.start_time = session.end_time - (minutes * 60);
    
    sessions.push_back(session);
    
    // Обновляем сегодняшнюю статистику
    updateTodayCache();
    today_usage[category] += minutes;
}

int UserStorage::getTodayMinutes(const std::string& category) {
    updateTodayCache();
    if (today_usage.find(category) != today_usage.end()) {
        return today_usage[category];
    }
    return 0;
}

int UserStorage::getRemainingMinutes(const std::string& category) {
    int limit = getLimit(category);
    if (limit == 0) return 0;
    
    int spent = getTodayMinutes(category);
    int remaining = limit - spent;
    return (remaining > 0) ? remaining : 0;
}

bool UserStorage::isLimitExceeded(const std::string& category) {
    int limit = getLimit(category);
    if (limit == 0) return false;
    return getTodayMinutes(category) > limit;
}

std::string UserStorage::getStatsString() {
    updateTodayCache();
    
    std::stringstream ss;
    
    ss << "========================================\n";
    ss << "Статистика за " << today_date << "\n";
    ss << "Пользователь: " << user_name << "\n";
    ss << "----------------------------------------\n";
    
    // Собираем все категории
    std::unordered_map<std::string, int> all_cats = today_usage;
    for (const auto& [cat, limit] : limits) {
        if (all_cats.find(cat) == all_cats.end()) {
            all_cats[cat] = 0;
        }
    }
    
    for (const auto& [category, spent] : all_cats) {
        int limit = getLimit(category);
        int remaining = (limit > spent) ? (limit - spent) : 0;
        
        ss << category << ":\n";
        ss << "  Потрачено: " << spent << " мин\n";
        if (limit > 0) {
            ss << "  Лимит: " << limit << " мин\n";
            ss << "  Осталось: " << remaining << " мин\n";
            if (spent > limit) {
                ss << "ПРЕВЫШЕНИЕ!\n";
            }
        }
        ss << "\n";
    }
    
    ss << "Всего сессий: " << sessions.size() << "\n";
    ss << "========================================";
    
    return ss.str();
}

void UserStorage::clearAll() {
    sessions.clear();
    today_usage.clear();
    limits.clear();
    today_date = getCurrentDate();
}