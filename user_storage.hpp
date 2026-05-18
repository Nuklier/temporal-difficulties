#ifndef USER_STORAGE_HPP
#define USER_STORAGE_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <ctime>

// Структура одной сессии
struct ActivitySession {
    std::string category;
    time_t start_time;
    time_t end_time;
    
    int getDurationMinutes() const {
        return static_cast<int>((end_time - start_time) / 60);
    }
};

// Класс для хранения данных
class UserStorage {
private:
    std::string user_name;
    
    // Лимиты: категория -> минуты
    std::unordered_map<std::string, int> limits;
    
    // Все сессии
    std::vector<ActivitySession> sessions;
    
    // Кэш сегодняшнего использования
    std::unordered_map<std::string, int> today_usage;
    std::string today_date;
    
    std::string getCurrentDate();
    void updateTodayCache();
    
public:
    UserStorage(const std::string& name);
    
    // Управление лимитами
    void setLimit(const std::string& category, int minutes);
    int getLimit(const std::string& category);
    
    // Добавить сессию (вызывается из sendStatistics)
    void addSession(const std::string& category, int minutes);
    
    // Получить статистику
    int getTodayMinutes(const std::string& category);
    int getRemainingMinutes(const std::string& category);
    bool isLimitExceeded(const std::string& category);
    
    // Показать статистику
    std::string getStatsString();
    
    // Очистить все данные (для тестов)
    void clearAll();
};

#endif