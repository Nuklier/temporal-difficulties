#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "user_choice.hpp"
#include "TimeTracker.hpp"
#include "notifier_h.hpp"
#include "user_storage_h.hpp"
#include "storage_bridge.hpp"
#include <string>
#include <vector>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <cctype>

class Button{
private:
    sf::RectangleShape shape;
    sf::Font font{"font.ttf"};
    sf::Text text{font};
    float x_int = 20.0f;
    float y_int = 10.0f;
    sf::Color normal_color = sf::Color(128, 128, 128);
    sf::Color hover_color = sf::Color(160, 160, 160);
    
public:
    Button(const std::string& button_text, float x, float y) {
        text.setFont(font);
        text.setString(sf::String::fromUtf8(button_text.begin(), button_text.end()));
        text.setCharacterSize(24);
        text.setFillColor(sf::Color::Black);
        // text resizing
        sf::FloatRect text_bounds = text.getLocalBounds();
        shape.setSize(sf::Vector2f(
            text_bounds.size.x + x_int * 2,
            text_bounds.size.y + y_int * 2
        ));
        shape.setFillColor(normal_color);
        shape.setPosition(sf::Vector2f(
            x + x_int,
            y + y_int
        ));
        centerText();
    }

    void centerText() {
        sf::FloatRect text_bounds = text.getLocalBounds();
        sf::Vector2f shape_pos = shape.getPosition();
        sf::Vector2f shape_size = shape.getSize();
        
        text.setPosition(sf::Vector2f(
            shape_pos.x + (shape_size.x - text_bounds.size.x) / 2 - text_bounds.position.x,
            shape_pos.y + (shape_size.y - text_bounds.size.y) / 2 - text_bounds.position.y
        ));
    }

    // text updating
    void setText(const std::string& new_text){
        text.setString(sf::String::fromUtf8(new_text.begin(), new_text.end()));
        sf::FloatRect text_bounds = text.getLocalBounds();
        shape.setSize(sf::Vector2f(
            text_bounds.size.x + x_int * 2,
            text_bounds.size.y + y_int * 2
        ));
        sf::Vector2f old_pos = shape.getPosition();
        text.setPosition(sf::Vector2f(
            old_pos.x + x_int,
            old_pos.y + y_int
        ));
        centerText();
    }

    // pos updating
    void setPosition(float x, float y){
        shape.setPosition(sf::Vector2f(x, y));
        centerText();
    }

    sf::Vector2f getPosition() const{
        return shape.getPosition();
    }

    sf::Vector2f getSize() const{
        return shape.getSize();
    }

    bool isPressed(const sf::RenderWindow& window, const sf::Event& event) const{
        if(const auto* mouse_pressed = event.getIf<sf::Event::MouseButtonPressed>()){
            if(mouse_pressed->button == sf::Mouse::Button::Left){
                sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
                return shape.getGlobalBounds().contains(sf::Vector2f(mouse_pos));
            }
        }
        return false;
    }

    bool isHovered(const sf::RenderWindow& window)const{
        sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
        return shape.getGlobalBounds().contains(sf::Vector2f(mouse_pos));
    }

    void update(const sf::RenderWindow& window){
        if (isHovered(window)){
            shape.setFillColor(hover_color);
        } else{
            shape.setFillColor(normal_color);
        }
    }

    void draw(sf::RenderWindow& window){
        window.draw(shape);
        window.draw(text);
    }
};


int main(){
    sf::RenderWindow window(sf::VideoMode({1280, 900}), "Time", sf::Style::Close, sf::State::Windowed);
    window.setFramerateLimit(60);
    sf::Font font("font.ttf");

    // normal objects init

    // unmapping your TimeTracker.hpp map into a vector for my private use
    std::vector<std::pair<std::string, std::string>> categories;
    for (const auto& category : Categories) {
        categories.push_back({category.first, category.second});
    }
    // Установка лимитов для категорий (пример)
    setCategoryLimit("сон", 1);
    setCategoryLimit("учеба", 1);
    setCategoryLimit("работа", 1);
    setCategoryLimit("развлечения", 1);
    setCategoryLimit("прием пищи", 1);
    setCategoryLimit("спорт", 1);
    setCategoryLimit("другое", 1);
    
    int actions_iterator = 0;
    bool action_chosen = false;
    std::string selected_category_tag = categories[6].first;

    TimeTracker tracker; // import #1
    bool was_tracking_active = false;
    
    bool input_mode = false;
    std::string input_buffer = "";
    int timer_hours = 0;
    
    bool show_tracker_status = false;

    enum class AppState {
        MAIN_MENU,
        TRACKER_SCREEN,
        STATS_SCREEN
    };
    AppState currentState = AppState::MAIN_MENU;

    // sfml objects init

    // plain text Text objects
    sf::Text greetings(font);
    greetings.setString(L"Нет времени объяснять,\nвремени вообще нет");
    greetings.setFillColor(sf::Color::Black);
    greetings.setPosition(sf::Vector2f(500, 10));

    sf::Text global_time(font);
    global_time.setPosition(sf::Vector2f(10, 800));
    global_time.setFillColor(sf::Color::Black);

    sf::Text tracker_status(font);
    tracker_status.setPosition(sf::Vector2f(10, 100));
    tracker_status.setCharacterSize(18);
    tracker_status.setFillColor(sf::Color(0, 0, 150));

    sf::Text stats(font);
    stats.setString("no stats yet");
    stats.setFillColor(sf::Color(255, 255, 255));
    stats.setPosition({600, 0});
    stats.setCharacterSize(15);

    // Button objects
    Button test("Время?", 500, 600); // twas a test button, but now it became a statistics one, now deprecated
    Button menuTrackerButton("Трекер", 540, 400);
    Button menuStatsButton("Статистика", 540, 500);
    Button backButton("Назад", 1000, 800);
    // tracker buttons bundle
    float act_choice_pos_x = 500;
    float act_choice_pos_y = 300;
    Button choice_itself(categories[6].second, act_choice_pos_x, act_choice_pos_y); // [6] - the programmer is eepy
    Button choice_left_arrow("<- ", act_choice_pos_x - 200, act_choice_pos_y - 10);
    Button choice_right_arrow(" ->", act_choice_pos_x + 200, act_choice_pos_y - 10);

    // lambda for choice_itself centering
    auto centerChoiceButton = [&]() {
        sf::Vector2u windowSize = window.getSize();
        float centerX = windowSize.x / 2.0f;
        float centerY = act_choice_pos_y;  // Сохраняем Y-позицию
        float buttonWidth = choice_itself.getSize().x;
        choice_itself.setPosition(centerX - buttonWidth / 2 - 75, centerY);
    };
    centerChoiceButton();


    sf::Text timer_settings(font);
    timer_settings.setPosition(sf::Vector2f(act_choice_pos_x - 100, act_choice_pos_y + 100));
    timer_settings.setString(L"Хочу таймер на минут");
    timer_settings.setFillColor(sf::Color(36, 17, 87));

    // main loop
    // | | | |
    // V V V V
    while(window.isOpen()){
        
        // event manager
        while(const std::optional event = window.pollEvent()){

            // close request
            if(event->is<sf::Event::Closed>()){
                window.close();
            }


            // p as "releeeasee meeee"
            if (const auto* key_event = event->getIf<sf::Event::KeyPressed>()) {
                if (key_event->scancode == sf::Keyboard::Scancode::P && tracker.isTrackingActive()) {
                    tracker.finishTracking();
                    timer_settings.setString(L"Таймер остановлен нажатием P");
                    timer_settings.setFillColor(sf::Color(200, 50, 50));
                    show_tracker_status = false;
                    action_chosen = false;
                    input_mode = false;
                    input_buffer.clear();
                }
            }

            // input mode logic
            if (input_mode) {
                if (const auto* text_event = event->getIf<sf::Event::TextEntered>()) {
                    char entered_char = static_cast<char>(text_event->unicode);
                    
                    if (std::isdigit(entered_char)) {
                        input_buffer += entered_char;
                        if (!input_buffer.empty()) {
                            timer_settings.setString(L"Хочу таймер на " + sf::String::fromUtf8(input_buffer.begin(), input_buffer.end()) + L" минут");
                        }
                    }
                }
                
                if (const auto* key_event = event->getIf<sf::Event::KeyPressed>()) {

                    // enter as "aight, i'll play your game for once"
                    if (key_event->scancode == sf::Keyboard::Scancode::Enter) {
                        if (!input_buffer.empty()) {
                            timer_hours = std::stoi(input_buffer);
                            timer_settings.setString(L"Таймер установлен на " + sf::String::fromUtf8(input_buffer.begin(), input_buffer.end()) + L" минут\nНажмите P для сброса");
                            timer_settings.setFillColor(sf::Color(0, 150, 0));
                            tracker.processTracking(true, selected_category_tag, timer_hours);
                            show_tracker_status = true;
                        }
                        input_mode = false;
                        input_buffer.clear();
                        action_chosen = false;
                    }

                    // backspace as "i changed my mind, how to redo"
                    if (key_event->scancode == sf::Keyboard::Scancode::Backspace) {
                        if (!input_buffer.empty()) {
                            input_buffer.pop_back();
                            if (!input_buffer.empty()) {
                                timer_settings.setString(L"Хочу таймер на " + sf::String::fromUtf8(input_buffer.begin(), input_buffer.end()) + L" минут");
                            } else {
                                timer_settings.setString(L"Хочу таймер на минут");
                                timer_settings.setFillColor(sf::Color(36, 17, 87));
                            }
                        }
                    }
                    
                    // escape as "nah, there is no time anyway"
                    if (key_event->scancode == sf::Keyboard::Scancode::Escape) {
                        input_mode = false;
                        input_buffer.clear();
                        action_chosen = false;
                        timer_settings.setString(L"Хочу таймер на минут");
                        timer_settings.setFillColor(sf::Color(36, 17, 87));
                    }
                }
            }

            // test Button, the one that is deprecated
            /*
            if(test.isPressed(window, *event)){
                std::string stats_string = getStatsString();
                stats.setString(sf::String::fromUtf8(stats_string.begin(), stats_string.end()));   
            }
                */

            // choice arrows
            if(choice_right_arrow.isPressed(window, *event)){
                if(actions_iterator == categories.size() - 1){
                    actions_iterator = 0;
                } else{
                    actions_iterator++;
                }
                choice_itself.setText(categories[actions_iterator].second);
                selected_category_tag = categories[actions_iterator].first;
                centerChoiceButton();
            }
             if(choice_left_arrow.isPressed(window, *event)){
                if(actions_iterator == 0){
                    actions_iterator = categories.size() - 1;
                } else{
                    actions_iterator--;
                }
                choice_itself.setText(categories[actions_iterator].second);
                selected_category_tag = categories[actions_iterator].first;
                centerChoiceButton();
            }
            
            if(choice_itself.isPressed(window, *event)){
                // blocking second-opinion input if tracker is already active
                if (tracker.isTrackingActive()) {
                    timer_settings.setString(L"Сначала остановите текущий таймер (P)");
                    timer_settings.setFillColor(sf::Color(200, 50, 50));
                } else if(!action_chosen){
                    input_mode = true;
                    input_buffer.clear();
                    timer_settings.setString(L"Введите количество минут:");
                    timer_settings.setFillColor(sf::Color(255, 200, 0));
                    action_chosen = true;
                } else{
                    timer_settings.setFillColor(sf::Color(36, 17, 87));
                    if (!input_mode) {
                        timer_settings.setString(L"Хочу таймер на минут");
                    }
                    action_chosen = false;
                    input_mode = false;
                    input_buffer.clear();
                }
            }

            // AppState
            if(currentState == AppState::MAIN_MENU){
                if(menuTrackerButton.isPressed(window, *event)){
                    currentState = AppState::TRACKER_SCREEN;
                }
                if(menuStatsButton.isPressed(window, *event)){
                    currentState = AppState::STATS_SCREEN;
                    std::string stats_string = getStatsString();
                    if (stats_string.empty()){
                        stats.setString(L"Нет данных");
                    } else{
                        stats.setString(sf::String::fromUtf8(stats_string.begin(), stats_string.end()));
                    }
                }
            }
            
            // backButton
            if(backButton.isPressed(window, *event)){
                if(currentState != AppState::MAIN_MENU){
                    currentState = AppState::MAIN_MENU;
                }
            }

        }

        // global_time update
        auto now = std::chrono::system_clock::now();
        std::time_t tt = std::chrono::system_clock::to_time_t(now);
        std::tm tm = *std::localtime(&tt);
        std::stringstream ss;
        ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
        std::string s = ss.str();
        global_time.setString(s);

        bool current_tracking_state = tracker.isTrackingActive();
    if (was_tracking_active && !current_tracking_state) {
        // tracker stopped by any means -> updating all flags
        timer_settings.setString(L"Хочу таймер на минут");
        timer_settings.setFillColor(sf::Color(36, 17, 87));
        show_tracker_status = false;
        action_chosen = false;
        input_mode = false;
        input_buffer.clear();
    }
    was_tracking_active = current_tracking_state;
        
        // tracker status
        if (show_tracker_status && tracker.isTrackingActive()) {
            tracker_status.setString(L"Трекер работает, и ты тоже иди работай");
            tracker_status.setFillColor(sf::Color(0, 150, 0));
        } else if (!tracker.isTrackingActive() && show_tracker_status) {
            tracker_status.setString(L"Трекер остановлен");
            tracker_status.setFillColor(sf::Color(150, 0, 0));
        }
        
        //Button's update
        test.update(window);
        choice_left_arrow.update(window);
        choice_right_arrow.update(window);
        choice_itself.update(window);
        menuTrackerButton.update(window);
        menuStatsButton.update(window);
        backButton.update(window);
        
        // i want to become a drawer
        window.clear(sf::Color{36, 17, 87});

        // buttons
        window.draw(global_time);
        
        if(currentState == AppState::MAIN_MENU){
            window.draw(greetings);
            menuTrackerButton.draw(window);
            menuStatsButton.draw(window);
        }
        else if(currentState == AppState::TRACKER_SCREEN){
            window.draw(timer_settings);
            choice_itself.draw(window);
            choice_left_arrow.draw(window);
            choice_right_arrow.draw(window);
            backButton.draw(window);
            if(show_tracker_status){
                window.draw(tracker_status);
            }
        }
        else if(currentState == AppState::STATS_SCREEN){
            window.draw(stats);
            backButton.draw(window);
        }

        window.display();
    }
    
    return 0;
}