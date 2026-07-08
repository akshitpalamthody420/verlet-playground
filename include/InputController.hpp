#pragma once

#include "World.hpp"

#include <SFML/Graphics.hpp>
#include <optional>

class InputController
{
public:
    void handleEvent(const sf::Event& event, sf::RenderWindow& window, World& world);
    void update(sf::RenderWindow& window, World& world);

    std::optional<std::size_t> hoveredParticle() const { return hoveredParticle_; }
    std::optional<std::size_t> draggedParticle() const { return draggedParticle_; }
    bool cutterActive() const { return cutterActive_; }
    Vec2 mouseWorld() const { return mouseWorld_; }

private:
    Vec2 mousePosition(sf::RenderWindow& window) const;

    std::optional<std::size_t> hoveredParticle_;
    std::optional<std::size_t> draggedParticle_;
    Vec2 mouseWorld_;
    bool cutterActive_ = false;
};
