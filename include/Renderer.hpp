#pragma once

#include "World.hpp"

#include <SFML/Graphics.hpp>
#include <optional>

class Renderer
{
public:
    explicit Renderer(sf::RenderWindow& window);

    void draw(const World& world,
              Vec2 mouseWorld,
              std::optional<std::size_t> hoveredParticle,
              std::optional<std::size_t> draggedParticle,
              bool cutterActive);

private:
    sf::RenderWindow& window_;
};
