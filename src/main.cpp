#include <algorithm>
#include <cmath>
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <numbers>
#include <optional>
#include <random>
#include <vector>

#include <SFML/Graphics.hpp>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
const float CIRCLE_RADIUS = 20.0f;
const int FPS_LIMIT = 30;
const float pi = std::numbers::pi;
const int GRAPH_LEFT_MARGIN = 100;
const int GRAPH_WIDTH = 600;
const int GRAPH_TOP = 480;
const int GRAPH_HEIGHT = 250;

// global tween function
std::function<float(float, float, float)> tween = [](float a, float b, float t) {
    return (1 - t) * a + t * b;
};

void handleInput(sf::Window& window, bool& shouldQuit) {
    while (const std::optional<sf::Event> event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
            shouldQuit = true;
        }

        // ====== ====== ======
        // TODO: (Q2)
        //  implement key presses (1-9) that replace the tween function
        //  with different alternate tween functions.
        //  Functions can be from lecture or from https://easings.net/#
        // ====== ====== ======
        if (const auto* keyEvent = event->getIf<sf::Event::KeyPressed>()) {
            switch (keyEvent->code) {
                case sf::Keyboard::Key::Num1:
                    // Linear
                    tween = [](float a, float b, float t) { return a + (b - a) * t; };
                    break;
                case sf::Keyboard::Key::Num2:
                    tween = [](float a, float b, float t) {
                        // Quadratic Ease In
                        return a + (b - a) * (t * t);
                    };
                    break;
                case sf::Keyboard::Key::Num3:
                    tween = [](float a, float b, float t) {
                        // Quadratic Ease Out
                        return a + (b - a) * (1 - (1 - t) * (1 - t));
                    };
                    break;
                case sf::Keyboard::Key::Num4:
                    tween = [](float a, float b, float t) {
                        // Quadratic Ease In-Out
                        if (t < 0.5f) {
                            return a + (b - a) * (2 * (t * t));
                        } else {
                            return a + (b - a) * ((1 - ((-2 * t + 2) * (-2 * t + 2)) / 2));
                        }
                    };
                    break;
                case sf::Keyboard::Key::Num5:
                    tween = [](float a, float b, float t) {
                        // Cubic Ease In
                        return a + (b - a) * (t * t * t);
                    };
                    break;
                case sf::Keyboard::Key::Num6:
                    tween = [](float a, float b, float t) {
                        // Cubic Ease Out
                        return a + (b - a) * (1 - ((1 - t) * (1 - t) * (1 - t)));
                    };
                    break;
                case sf::Keyboard::Key::Num7:
                    tween = [](float a, float b, float t) {
                        // Cubic Ease In-Out
                        if (t < 0.5f) {
                            return a + (b - a) * (4 * (t * t * t));
                        } else {
                            return a + (b - a) *
                                           ((1 - ((-2 * t + 2) * (-2 * t + 2) * (-2 * t + 2)) / 2));
                        }
                    };
                    break;
                case sf::Keyboard::Key::Num8:
                    tween = [](float a, float b, float t) {
                        // Sine Ease In
                        return a + (b - a) * (1 - std::cos(t * pi / 2));
                    };
                    break;
                case sf::Keyboard::Key::Num9:
                    tween = [](float a, float b, float t) {
                        // Sine Ease Out
                        return a + (b - a) * std::sin(t * pi / 2);
                    };
                    break;
                default:
                    break;
            }
        }
    }
}

void render(sf::RenderWindow& window) {
    // Clear with blue background (sky)
    window.clear(sf::Color::Black);
    // ====== ====== ======
    // TODO: (Q1) Draw circle that moves between
    // the left/right half of the screen.
    // Movement should be governed by the tween function.
    // ====== ====== ======
    static const int FRAMES_PER_ANIMATION = 60;
    static int counter = 0;
    float t = (counter % FRAMES_PER_ANIMATION) / (FRAMES_PER_ANIMATION - 1.0f);
    float y = WINDOW_HEIGHT / 3.0f;
    float x = tween(CIRCLE_RADIUS, WINDOW_WIDTH - CIRCLE_RADIUS, t);
    counter++;

    sf::CircleShape circle;
    circle.setRadius(CIRCLE_RADIUS);
    circle.setOrigin({CIRCLE_RADIUS, CIRCLE_RADIUS});
    circle.setPosition({x, y});
    circle.setFillColor(sf::Color::White);

    window.draw(circle);

    // ====== ====== ======
    // TODO: (Q3) Draw tween function graph with a dot
    // on the current portion of the curve
    // ====== ====== ======
    // Helper function to convert from
    // Normalized coordinates (0-1)
    // to screen coordinates
    auto toScreen = [](float px, float py) {
        return sf::Vector2f{GRAPH_LEFT_MARGIN + px * GRAPH_WIDTH,
                            GRAPH_TOP + GRAPH_HEIGHT - py * GRAPH_HEIGHT};
    };

    // Axes
    sf::VertexArray axes(sf::PrimitiveType::Lines, 4);
    // origin
    axes[0] = {toScreen(0, 0), sf::Color::White};
    // x axis
    axes[1] = {toScreen(1, 0), sf::Color::White};
    // y axis
    axes[2] = {toScreen(0, 0), sf::Color::White};
    // (0,1)
    axes[3] = {toScreen(0, 1), sf::Color::White};
    window.draw(axes);

    // Curve
    sf::VertexArray curve(sf::PrimitiveType::LineStrip);
    const int SAMPLES = 100;
    // Draw curve with 100 samples
    for (int i = 0; i <= SAMPLES; i++) {
        // sx is the normalized x coordinate
        float sx = i / static_cast<float>(SAMPLES);
        // py is the normalized y coordinate
        float py = tween(0, 1, sx);
        curve.append({toScreen(sx, py), sf::Color::White});
    }
    window.draw(curve);

    // Dot at Current t
    sf::CircleShape dot(6.0f);
    dot.setOrigin({6.0f, 6.0f});
    dot.setPosition(toScreen(t, tween(0, 1, t)));
    dot.setFillColor(sf::Color::White);
    window.draw(dot);
    window.display();
}

int main() {
    sf::RenderWindow window;

    try {
        // Initialize window
        window.create(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Tween");
        window.setFramerateLimit(FPS_LIMIT);
        // Prevent key repeats.
        window.setKeyRepeatEnabled(false);

        bool shouldQuit = false;
        // Main game loop
        while (window.isOpen()) {
            handleInput(window, shouldQuit);
            if (shouldQuit) {
                break;
            }
            render(window);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}
