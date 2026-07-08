#include "InputController.hpp"

Vec2 InputController::mousePosition(sf::RenderWindow& window) const
{
    const sf::Vector2i pixel = sf::Mouse::getPosition(window);
    const sf::Vector2f world = window.mapPixelToCoords(pixel);
    return {world.x, world.y};
}

void InputController::handleEvent(const sf::Event& event, sf::RenderWindow& window, World& world)
{
    mouseWorld_ = mousePosition(window);

    if (event.type == sf::Event::MouseButtonPressed)
    {
        if (event.mouseButton.button == sf::Mouse::Left)
        {
            draggedParticle_ = world.findNearestParticle(mouseWorld_, 45.0f);
        }
        else if (event.mouseButton.button == sf::Mouse::Right)
        {
            const auto nearest = world.findNearestParticle(mouseWorld_, 50.0f);
            if (nearest)
            {
                world.togglePinned(*nearest);
            }
        }
        else if (event.mouseButton.button == sf::Mouse::Middle)
        {
            cutterActive_ = true;
            world.cutConstraintsNear(mouseWorld_, 28.0f);
        }
    }

    if (event.type == sf::Event::MouseButtonReleased)
    {
        if (event.mouseButton.button == sf::Mouse::Left)
        {
            draggedParticle_.reset();
        }
        else if (event.mouseButton.button == sf::Mouse::Middle)
        {
            cutterActive_ = false;
        }
    }

    if (event.type == sf::Event::KeyPressed)
    {
        if (event.key.code == sf::Keyboard::Space)
        {
            world.createRope(mouseWorld_, 26, 20.0f, true);
        }
        else if (event.key.code == sf::Keyboard::C)
        {
            world.createCloth(mouseWorld_ - Vec2{250.0f, 35.0f}, 22, 13, 24.0f);
        }
        else if (event.key.code == sf::Keyboard::B)
        {
            world.createSoftBody(mouseWorld_, 75.0f, 18);
        }
        else if (event.key.code == sf::Keyboard::W)
        {
            world.windEnabled = !world.windEnabled;
        }
        else if (event.key.code == sf::Keyboard::T)
        {
            world.tearingEnabled = !world.tearingEnabled;
        }
        else if (event.key.code == sf::Keyboard::G)
        {
            world.gravityEnabled = !world.gravityEnabled;
        }
        else if (event.key.code == sf::Keyboard::R)
        {
            world.createDemoScene();
        }
        else if (event.key.code == sf::Keyboard::Delete || event.key.code == sf::Keyboard::BackSpace)
        {
            world.clear();
        }
    }
}

void InputController::update(sf::RenderWindow& window, World& world)
{
    mouseWorld_ = mousePosition(window);
    hoveredParticle_ = world.findNearestParticle(mouseWorld_, 35.0f);

    if (draggedParticle_)
    {
        world.setParticlePosition(*draggedParticle_, mouseWorld_);
    }

    cutterActive_ = sf::Mouse::isButtonPressed(sf::Mouse::Middle);
    if (cutterActive_)
    {
        world.cutConstraintsNear(mouseWorld_, 28.0f);
    }
}
