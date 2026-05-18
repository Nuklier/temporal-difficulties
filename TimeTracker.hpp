#ifndef TIME_TRACKER_H
#define TIME_TRACKER_H

#include <chrono>
#include <string>
#include <unordered_map>
#include <thread>
#include <atomic>
#include <memory>
#include "user_choice.hpp"

const std::unordered_map<std::string, std::string> Categories = {
    {"btn_sleep", "сон"},
    {"btn_study", "учеба"},
    {"btn_work", "работа"},
    {"btn_entertainment", "развлечения"},
    {"btn_eating", "прием пищи"},
    {"btn_sport", "спорт"},
    {"btn_other", "другое"}
};

class TimeTracker {
private:
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point finishTime;
    std::atomic<bool> isActive{false};
    std::atomic<bool> shouldStop{false};
    std::string currentCategory;
    int currentLimit;
    std::unique_ptr<std::thread> trackingThread;
    
    std::string getCategoryByTag(const std::string& tag) const {
        return Categories.at(tag);
    }
    
    // Отдельный метод для отправки статистики
    void sendCurrentStatistics() {
        if (startTime.time_since_epoch().count() != 0) {
            finishTime = std::chrono::system_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::minutes>(finishTime - startTime).count();
            if (duration > 0) {
                sendStatistics(currentCategory, static_cast<int>(duration));
            }
        }
    }
    
public:
    TimeTracker() : currentLimit(0) {}
    
    ~TimeTracker() {
        finishTracking();
    }
    
    void processTracking(bool start_on, const std::string& tag, int limit) {
        if (start_on) {
            // Останавливаем предыдущий трекер если был
            if (isActive || (trackingThread && trackingThread->joinable())) {
                shouldStop = true;
                isActive = false;
                
                if (trackingThread && trackingThread->joinable()) {
                    trackingThread->join();
                }
                trackingThread.reset();
            }
            
            currentCategory = getCategoryByTag(tag);
            startTime = std::chrono::system_clock::now();
            currentLimit = limit;
            isActive = true;
            shouldStop = false;
            
            trackingThread = std::make_unique<std::thread>([this]() {
                auto limitEndTime = startTime + std::chrono::minutes(currentLimit);
                
                while (isActive && !shouldStop) {
                    auto currentTime = std::chrono::system_clock::now();
                    
                    if (currentTime > limitEndTime) {
                        UserChoice userChoice = showNotification(startTime, currentCategory, currentLimit);
                        
                        if (userChoice == UserChoice::STOP_ACTIVITY) {
                            // Останавливаем трекер и отправляем статистику
                            shouldStop = true;
                            isActive = false;
                            // Отправляем статистику из потока
                            sendCurrentStatistics();
                            break;
                        } else {
                            limitEndTime += std::chrono::minutes(1);
                        }
                    }
                    
                    for (int i = 0; i < 60 && isActive && !shouldStop; ++i) {
                        std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    }
                }
            });
        }
    }
    
    void finishTracking() { 
        if (isActive) {
            shouldStop = true;
            isActive = false;
            
            if (trackingThread && trackingThread->joinable()) {
                trackingThread->join();
            }
            // Отправляем статистику при ручной остановке
            sendCurrentStatistics();
        }
        trackingThread.reset();
    }
    
    bool isTrackingActive() const {
        return isActive;
    }
    
    std::string getCurrentCategory() const {
        return currentCategory;
    }
    
    int getElapsedMinutes() const {
        if (!isActive) return 0;
        auto now = std::chrono::system_clock::now();
        return static_cast<int>(std::chrono::duration_cast<std::chrono::minutes>(now - startTime).count());
    }
};

#endif // TIME_TRACKER_H