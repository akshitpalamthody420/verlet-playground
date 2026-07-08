#include "InputController.hpp"
#include "Renderer.hpp"
#include "World.hpp"

#include <SFML/Graphics.hpp>
#include <sstream>

int main()
{
    constexpr unsigned int windowWidth = 1100;
    constexpr unsigned int windowHeight = 850;
    constexpr unsigned int frameRate = 120;
    constexpr float fixedTimeStep = 1.0f / 120.0f;

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Verlet Playground");
    window.setFramerateLimit(frameRate);

    World world(static_cast<float>(windowWidth), static_cast<float>(windowHeight));
    world.createDemoScene();

    Renderer renderer(window);
    InputController input;

    sf::Clock clock;
    sf::Clock titleClock;
    float accumulator = 0.0f;
    float elapsedTime = 0.0f;

    while (window.isOpen())
    {
        const float frameTime = std::min(clock.restart().asSeconds(), 0.05f);
        accumulator += frameTime;
        elapsedTime += frameTime;

        sf::Event event{};
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
            {
                window.close();
            }
            else
            {
                input.handleEvent(event, window, world);
            }
        }

        input.update(window, world);

        while (accumulator >= fixedTimeStep)
        {
            world.step(fixedTimeStep, elapsedTime);
            accumulator -= fixedTimeStep;
        }

        if (titleClock.getElapsedTime().asSeconds() > 0.25f)
        {
            std::ostringstream title;
            title << "Verlet Playground | particles: " << world.particles().size()
                  << " | constraints: " << world.constraints().size()
                  << " | W wind: " << (world.windEnabled ? "on" : "off")
                  << " | T tear: " << (world.tearingEnabled ? "on" : "off")
                  << " | G gravity: " << (world.gravityEnabled ? "on" : "off")
                  << " | LMB drag | RMB pin | MMB cut";
            window.setTitle(title.str());
            titleClock.restart();
        }

        window.clear(sf::Color(46, 42, 46));
        renderer.draw(world,
                      input.mouseWorld(),
                      input.hoveredParticle(),
                      input.draggedParticle(),
                      input.cutterActive());
        window.display();
    }

    return 0;
}
