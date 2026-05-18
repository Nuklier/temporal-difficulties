#include "notifier_l.hpp"

#include <SFML/Graphics.hpp>
#include <chrono>
#include <string>

namespace {
    sf::String toSfString(const std::string& text)
    {
        return sf::String::fromUtf8(text.begin(), text.end());
    }
}

UserChoice showNotification(
    std::chrono::system_clock::time_point startTime,
    const std::string& category,
    int limit)
{
    auto now = std::chrono::system_clock::now();

    auto duration = now - startTime;

    int elapsedMinutes = static_cast<int>(
        std::chrono::duration_cast<std::chrono::minutes>(duration).count()
    );

    int excessMinutes = 0;

    if (elapsedMinutes > limit) {
        excessMinutes = elapsedMinutes - limit;
    }

    const int windowWidth = 720;
    const int windowHeight = 430;

    sf::ContextSettings settings;
    settings.antiAliasingLevel = 8;

    sf::RenderWindow window(
        sf::VideoMode({windowWidth, windowHeight}),
        L"Контроль времени",
        sf::Style::Close,
        sf::State::Windowed,
        settings
    );

    window.setFramerateLimit(60);

    sf::Font font;

    if (!font.openFromFile("assets/font.ttf")) {
        if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
            return UserChoice::STOP_ACTIVITY;
        }
    }

    sf::RectangleShape background(sf::Vector2f(windowWidth, windowHeight));
    background.setFillColor(sf::Color(18, 22, 34));

    sf::CircleShape glow1(180);
    glow1.setFillColor(sf::Color(55, 95, 190, 55));
    glow1.setPosition(sf::Vector2f(-80, -100));

    sf::CircleShape glow2(150);
    glow2.setFillColor(sf::Color(210, 70, 100, 45));
    glow2.setPosition(sf::Vector2f(560, 250));

    sf::RectangleShape card(sf::Vector2f(640, 330));
    card.setFillColor(sf::Color(29, 35, 52));
    card.setPosition(sf::Vector2f(40, 50));

    sf::RectangleShape accent(sf::Vector2f(6, 330));
    accent.setFillColor(sf::Color(95, 135, 255));
    accent.setPosition(sf::Vector2f(40, 50));

    sf::RectangleShape badge(sf::Vector2f(185, 34));
    badge.setFillColor(sf::Color(55, 66, 95));
    badge.setPosition(sf::Vector2f(72, 78));

    sf::Text badgeText(font);
    badgeText.setString(toSfString("ПРЕВЫШЕН ЛИМИТ"));
    badgeText.setCharacterSize(14);
    badgeText.setFillColor(sf::Color(165, 185, 255));
    badgeText.setPosition(sf::Vector2f(88, 85));

    sf::Text title(font);
    title.setString(toSfString("Превышен лимит времени"));
    title.setCharacterSize(28);
    title.setFillColor(sf::Color::White);
    title.setPosition(sf::Vector2f(72, 128));

    sf::Text subtitle(font);
    subtitle.setString(
        toSfString("Вы провели в этой категории больше времени, чем разрешено.")
    );
    subtitle.setCharacterSize(17);
    subtitle.setFillColor(sf::Color(175, 184, 205));
    subtitle.setPosition(sf::Vector2f(72, 170));

    sf::RectangleShape infoPanel(sf::Vector2f(580, 92));
    infoPanel.setFillColor(sf::Color(23, 28, 42));
    infoPanel.setPosition(sf::Vector2f(72, 215));

    std::string detailsText =
        "Категория: " + category + "\n"
        "Прошло: " + std::to_string(elapsedMinutes) + " мин     "
        "Лимит: " + std::to_string(limit) + " мин     "
        "Превышение: " + std::to_string(excessMinutes) + " мин";

    sf::Text details(font);
    details.setString(toSfString(detailsText));
    details.setCharacterSize(17);
    details.setFillColor(sf::Color(220, 225, 240));
    details.setLineSpacing(1.45f);
    details.setPosition(sf::Vector2f(95, 235));

    const int buttonWidth = 170;
    const int buttonHeight = 48;
    const int spacing = 24;

    const int totalWidth = buttonWidth * 2 + spacing;
    const int startX = (windowWidth - totalWidth) / 2;
    const int buttonY = 318;

    sf::RectangleShape continueButton(
        sf::Vector2f(buttonWidth, buttonHeight)
    );

    continueButton.setFillColor(sf::Color(84, 125, 255));
    continueButton.setPosition(sf::Vector2f(startX, buttonY));

    sf::Text continueText(font);
    continueText.setString(toSfString("Продолжить"));
    continueText.setCharacterSize(17);
    continueText.setFillColor(sf::Color::White);

    sf::FloatRect continueBounds = continueText.getLocalBounds();

    continueText.setPosition({
        startX + (buttonWidth - continueBounds.size.x) / 2,
        buttonY + (buttonHeight - continueBounds.size.y) / 2 - 6
 });

    sf::RectangleShape stopButton(
        sf::Vector2f(buttonWidth, buttonHeight)
    );

    stopButton.setFillColor(sf::Color(226, 86, 95));
    stopButton.setPosition(sf::Vector2f(startX + buttonWidth + spacing, buttonY));

    sf::Text stopText(font);
    stopText.setString(toSfString("Остановить"));
    stopText.setCharacterSize(17);
    stopText.setFillColor(sf::Color::White);

    sf::FloatRect stopBounds = stopText.getLocalBounds();

    stopText.setPosition({
        startX + buttonWidth + spacing + (buttonWidth - stopBounds.size.x) / 2,
        buttonY + (buttonHeight - stopBounds.size.y) / 2 - 6
    });

    UserChoice result = UserChoice::REMIND_LATER;

    while (window.isOpen()) {


        while (const std::optional event = window.pollEvent()) {

            if (event->is<sf::Event::Closed>()) {
                result = UserChoice::STOP_ACTIVITY;
                window.close();
            }

            if (event->is<sf::Event::MouseButtonPressed>() &&
                sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {

                sf::Vector2i mousePos = sf::Mouse::getPosition(window);

                if (continueButton.getGlobalBounds().contains(sf::Vector2f(
                        static_cast<float>(mousePos.x),
                        static_cast<float>(mousePos.y)))) {

                    result = UserChoice::REMIND_LATER;
                    window.close();
                }

                if (stopButton.getGlobalBounds().contains(sf::Vector2f(
                        static_cast<float>(mousePos.x),
                        static_cast<float>(mousePos.y)))) {

                    result = UserChoice::STOP_ACTIVITY;
                    window.close();
                }
            }
        }

        window.clear();

        window.draw(background);
        window.draw(glow1);
        window.draw(glow2);

        window.draw(card);
        window.draw(accent);

        window.draw(badge);
        window.draw(badgeText);

        window.draw(title);
        window.draw(subtitle);

        window.draw(infoPanel);
        window.draw(details);

        window.draw(continueButton);
        window.draw(continueText);

        window.draw(stopButton);
        window.draw(stopText);

        window.display();
    }

    return result;
}
