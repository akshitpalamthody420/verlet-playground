#include "Renderer.hpp"

namespace
{
sf::Vector2f toSf(Vec2 v)
{
    return {v.x, v.y};
}
}

Renderer::Renderer(sf::RenderWindow& window) : window_(window)
{
}

void Renderer::draw(const World& world,
                    Vec2 mouseWorld,
                    std::optional<std::size_t> hoveredParticle,
                    std::optional<std::size_t> draggedParticle,
                    bool cutterActive)
{
    sf::VertexArray lines(sf::Lines);

    for (const auto& constraint : world.constraints())
    {
        if (!constraint.active || constraint.a >= world.particles().size() || constraint.b >= world.particles().size())
        {
            continue;
        }

        const auto& a = world.particles()[constraint.a];
        const auto& b = world.particles()[constraint.b];
        lines.append(sf::Vertex(toSf(a.position), sf::Color(230, 230, 230, 160)));
        lines.append(sf::Vertex(toSf(b.position), sf::Color(230, 230, 230, 160)));
    }

    window_.draw(lines);

    sf::CircleShape particleShape;
    particleShape.setOrigin(1.0f, 1.0f);

    for (std::size_t i = 0; i < world.particles().size(); ++i)
    {
        const auto& particle = world.particles()[i];
        particleShape.setRadius(particle.radius);
        particleShape.setOrigin(particle.radius, particle.radius);
        particleShape.setPosition(toSf(particle.position));

        if (draggedParticle && *draggedParticle == i)
        {
            particleShape.setFillColor(sf::Color(255, 201, 0));
        }
        else if (particle.pinned)
        {
            particleShape.setFillColor(sf::Color(249, 38, 114));
        }
        else if (hoveredParticle && *hoveredParticle == i)
        {
            particleShape.setFillColor(sf::Color(61, 209, 119));
        }
        else
        {
            particleShape.setFillColor(sf::Color(0, 176, 255));
        }

        window_.draw(particleShape);
    }

    sf::CircleShape cursor;
    cursor.setRadius(cutterActive ? 26.0f : 12.0f);
    cursor.setOrigin(cursor.getRadius(), cursor.getRadius());
    cursor.setPosition(toSf(mouseWorld));
    cursor.setFillColor(sf::Color::Transparent);
    cursor.setOutlineThickness(2.0f);
    cursor.setOutlineColor(cutterActive ? sf::Color(255, 63, 128, 220) : sf::Color(254, 254, 254, 90));
    window_.draw(cursor);
}
