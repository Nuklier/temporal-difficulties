#include "user_storage.hpp"
#include <string>

// Глобальное хранилище
static UserStorage storage("Пользователь");

// ЭТУ ФУНКЦИЮ ВЫЗЫВАЕТ TimeTracker
void sendStatistics(std::string category, int minutes) {
    storage.addSession(category, minutes);
}

// Вспомогательные функции
void setCategoryLimit(const std::string& category, int minutes) {
    storage.setLimit(category, minutes);
}

// Возвращает строку со статистикой вместо вывода в консоль
std::string getStatsString() {
    return storage.getStatsString();
}

int getTimeSpentToday(const std::string& category) {
    return storage.getTodayMinutes(category);
}

int getRemainingTime(const std::string& category) {
    return storage.getRemainingMinutes(category);
}

bool isLimitExceeded(const std::string& category) {
    return storage.isLimitExceeded(category);
}

void clearAllData() {
    storage.clearAll();
}