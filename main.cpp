#include <SFML/Graphics.hpp>
#include <optional>
#include <vector>
#include <cmath>
#include <algorithm>
#include <string>

int main() {
    sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "SFML Lab - Variant 19: Forced Pendulum");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.openFromFile("../resources/fonts/arial.ttf")) {
    }

    sf::Text infoText(font, "", 18);
    infoText.setFillColor(sf::Color::White);
    infoText.setPosition({ 20.f, 20.f });
    const sf::Vector2f pivot(400.f, 150.f); 
    float length = 200.f;                   
    float angle = 1.0f;                     
    float angularVelocity = 0.f;            
    float gravity = 980.f;                  
    float damping = 0.5f;                   
    float forceAmplitude = 200.f;   
    float forceFrequency = 3.0f;    
    float totalTime = 0.f;          
    bool isRunning = true;
    sf::CircleShape bob(15.f);
    bob.setFillColor(sf::Color::Green);
    bob.setOrigin({ 15.f, 15.f });
    sf::RectangleShape line({ 2.f, length });
    line.setFillColor(sf::Color::White);
    line.setOrigin({ 1.f, 0.f });
    line.setPosition(pivot);
    std::vector<sf::Vector2f> trajectory;
    sf::VertexArray trajectoryLine(sf::PrimitiveType::LineStrip);
    sf::Clock clock;

    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (event->is<sf::Event::KeyPressed>()) {
                const auto key = event->getIf<sf::Event::KeyPressed>();
                if (key->code == sf::Keyboard::Key::Up) {
                    forceAmplitude += 50.f; 
                }
                if (key->code == sf::Keyboard::Key::Down) {
                    forceAmplitude -= 50.f; 
                }
                if (key->code == sf::Keyboard::Key::Right) {
                    forceFrequency += 0.5f; 
                }
                if (key->code == sf::Keyboard::Key::Left) {
                    forceFrequency -= 0.5f; 
                }
                if (key->code == sf::Keyboard::Key::Space) {
                    isRunning = !isRunning; 
                }
            }
        }

        forceAmplitude = std::clamp(forceAmplitude, 0.f, 1000.f);
        forceFrequency = std::clamp(forceFrequency, 0.0f, 20.f);

        float dt = clock.restart().asSeconds();
        if (dt > 0.1f) dt = 0.1f; 

        if (isRunning) {
            totalTime += dt;

            float externalForce = (forceAmplitude / length) * std::sin(forceFrequency * totalTime);

            float angularAcceleration = -(gravity / length) * std::sin(angle) - damping * angularVelocity + externalForce;

            angularVelocity += angularAcceleration * dt;
            angle += angularVelocity * dt;
        }

        float bobX = pivot.x + length * std::sin(angle);
        float bobY = pivot.y + length * std::cos(angle);
        sf::Vector2f bobPosition(bobX, bobY);
        bob.setPosition(bobPosition);
        line.setRotation(sf::radians(angle)); 
        trajectory.push_back(bobPosition);
        if (trajectory.size() > 300) {
            trajectory.erase(trajectory.begin());
        }

        trajectoryLine.clear();
        for (const auto& pt : trajectory) {
            trajectoryLine.append(sf::Vertex{ pt, sf::Color(150, 150, 150, 150) });
        }

        infoText.setString(
            "Status: " + std::string(isRunning ? "Running" : "Paused (Space)") + "\n" +
            "Amplitude (Up/Down): " + std::to_string(forceAmplitude) + "\n" +
            "Frequency (Left/Right): " + std::to_string(forceFrequency) + "\n" +
            "Angle (rad): " + std::to_string(angle) + "\n" +
            "Time: " + std::to_string(totalTime)
        );

        window.clear(sf::Color::Black);

        window.draw(trajectoryLine);
        window.draw(line);
        window.draw(bob);
        window.draw(infoText);
        window.display();
    }

    return 0;
}