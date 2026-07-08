#include "World.hpp"

#include <algorithm>
#include <cmath>

namespace
{
float distancePointToSegment(Vec2 point, Vec2 a, Vec2 b)
{
    const Vec2 ab = b - a;
    const float abLengthSquared = ab.lengthSquared();
    if (abLengthSquared <= 0.0001f)
    {
        return (point - a).length();
    }

    float t = ((point.x - a.x) * ab.x + (point.y - a.y) * ab.y) / abLengthSquared;
    t = std::max(0.0f, std::min(1.0f, t));
    const Vec2 projection = a + ab * t;
    return (point - projection).length();
}
}

World::World(float width, float height) : width_(width), height_(height)
{
}

std::size_t World::addParticle(Vec2 position, float radius, bool pinned)
{
    particles_.emplace_back(position, radius, pinned);
    return particles_.size() - 1;
}

void World::addConstraint(std::size_t a, std::size_t b, float stiffness, float tearMultiplier)
{
    if (a >= particles_.size() || b >= particles_.size() || a == b)
    {
        return;
    }

    const float length = (particles_[b].position - particles_[a].position).length();
    constraints_.emplace_back(a, b, length, stiffness, length * tearMultiplier);
}

void World::step(float dt, float timeSeconds)
{
    integrate(dt, timeSeconds);

    for (int i = 0; i < constraintIterations; ++i)
    {
        solveConstraints();
        keepInsideBounds();
    }

    removeInactiveConstraints();
}

void World::integrate(float dt, float timeSeconds)
{
    const float dtSquared = dt * dt;

    for (auto& particle : particles_)
    {
        if (particle.pinned)
        {
            particle.previousPosition = particle.position;
            particle.acceleration = {0.0f, 0.0f};
            continue;
        }

        Vec2 acceleration = {0.0f, 0.0f};
        if (gravityEnabled)
        {
            acceleration += gravity_;
        }

        if (windEnabled)
        {
            const float wave = std::sin(timeSeconds * 2.4f + particle.position.y * 0.025f);
            acceleration.x += wave * 850.0f;
        }

        const Vec2 velocity = (particle.position - particle.previousPosition) * drag_;
        particle.previousPosition = particle.position;
        particle.position += velocity + acceleration * dtSquared;
        particle.acceleration = {0.0f, 0.0f};
    }
}

void World::solveConstraints()
{
    for (auto& constraint : constraints_)
    {
        if (!constraint.active || constraint.a >= particles_.size() || constraint.b >= particles_.size())
        {
            constraint.active = false;
            continue;
        }

        Particle& a = particles_[constraint.a];
        Particle& b = particles_[constraint.b];
        const Vec2 delta = b.position - a.position;
        const float distance = delta.length();

        if (distance <= 0.0001f)
        {
            continue;
        }

        if (tearingEnabled && distance > constraint.tearLength)
        {
            constraint.active = false;
            continue;
        }

        const float difference = (distance - constraint.restLength) / distance;
        const Vec2 correction = delta * (constraint.stiffness * difference);

        if (!a.pinned && !b.pinned)
        {
            a.position += correction * 0.5f;
            b.position -= correction * 0.5f;
        }
        else if (a.pinned && !b.pinned)
        {
            b.position -= correction;
        }
        else if (!a.pinned && b.pinned)
        {
            a.position += correction;
        }
    }
}

void World::keepInsideBounds()
{
    for (auto& particle : particles_)
    {
        const Vec2 velocity = particle.position - particle.previousPosition;
        const float r = particle.radius;

        if (particle.position.x < r)
        {
            particle.position.x = r;
            particle.previousPosition.x = particle.position.x + velocity.x * bounce_;
        }
        else if (particle.position.x > width_ - r)
        {
            particle.position.x = width_ - r;
            particle.previousPosition.x = particle.position.x + velocity.x * bounce_;
        }

        if (particle.position.y < r)
        {
            particle.position.y = r;
            particle.previousPosition.y = particle.position.y + velocity.y * bounce_;
        }
        else if (particle.position.y > height_ - r)
        {
            particle.position.y = height_ - r;
            particle.previousPosition.y = particle.position.y + velocity.y * bounce_;
        }
    }
}

void World::removeInactiveConstraints()
{
    constraints_.erase(
        std::remove_if(constraints_.begin(), constraints_.end(), [](const Constraint& constraint) {
            return !constraint.active;
        }),
        constraints_.end());
}

void World::clear()
{
    particles_.clear();
    constraints_.clear();
}

void World::createDemoScene()
{
    clear();
    createCloth({120.0f, 90.0f}, 24, 14, 24.0f);
    createRope({760.0f, 100.0f}, 24, 20.0f, true);
    createSoftBody({790.0f, 520.0f}, 86.0f, 18);
}

void World::createRope(Vec2 start, int segments, float segmentLength, bool pinStart)
{
    if (segments < 2)
    {
        return;
    }

    std::size_t previous = addParticle(start, 6.0f, pinStart);
    for (int i = 1; i < segments; ++i)
    {
        const Vec2 position = {start.x + static_cast<float>(i) * segmentLength, start.y};
        const std::size_t current = addParticle(position, 6.0f, false);
        addConstraint(previous, current, 1.0f, 2.8f);
        previous = current;
    }
}

void World::createCloth(Vec2 topLeft, int columns, int rows, float spacing)
{
    if (columns < 2 || rows < 2)
    {
        return;
    }

    const std::size_t baseIndex = particles_.size();

    for (int y = 0; y < rows; ++y)
    {
        for (int x = 0; x < columns; ++x)
        {
            const bool pinned = (y == 0 && x % 4 == 0);
            addParticle({topLeft.x + static_cast<float>(x) * spacing,
                         topLeft.y + static_cast<float>(y) * spacing},
                        4.3f,
                        pinned);
        }
    }

    auto index = [baseIndex, columns](int x, int y) {
        return baseIndex + static_cast<std::size_t>(y * columns + x);
    };

    for (int y = 0; y < rows; ++y)
    {
        for (int x = 0; x < columns; ++x)
        {
            if (x < columns - 1)
            {
                addConstraint(index(x, y), index(x + 1, y), 0.98f, 2.05f);
            }
            if (y < rows - 1)
            {
                addConstraint(index(x, y), index(x, y + 1), 0.98f, 2.05f);
            }
            if (x < columns - 1 && y < rows - 1)
            {
                addConstraint(index(x, y), index(x + 1, y + 1), 0.35f, 2.4f);
                addConstraint(index(x + 1, y), index(x, y + 1), 0.35f, 2.4f);
            }
        }
    }
}

void World::createSoftBody(Vec2 center, float radius, int points)
{
    if (points < 6)
    {
        return;
    }

    const std::size_t centerIndex = addParticle(center, 7.0f, false);
    std::vector<std::size_t> ring;
    ring.reserve(static_cast<std::size_t>(points));

    constexpr float pi = 3.14159265358979323846f;
    for (int i = 0; i < points; ++i)
    {
        const float angle = (static_cast<float>(i) / static_cast<float>(points)) * 2.0f * pi;
        const Vec2 position = {center.x + std::cos(angle) * radius,
                               center.y + std::sin(angle) * radius};
        ring.push_back(addParticle(position, 6.0f, false));
    }

    for (int i = 0; i < points; ++i)
    {
        const std::size_t current = ring[static_cast<std::size_t>(i)];
        const std::size_t next = ring[static_cast<std::size_t>((i + 1) % points)];
        const std::size_t opposite = ring[static_cast<std::size_t>((i + points / 2) % points)];

        addConstraint(current, next, 0.95f, 2.6f);
        addConstraint(centerIndex, current, 0.65f, 2.8f);
        if (i < points / 2)
        {
            addConstraint(current, opposite, 0.35f, 2.8f);
        }
    }
}

std::optional<std::size_t> World::findNearestParticle(Vec2 point, float maxDistance) const
{
    const float maxDistanceSquared = maxDistance * maxDistance;
    float bestDistanceSquared = maxDistanceSquared;
    std::optional<std::size_t> bestIndex;

    for (std::size_t i = 0; i < particles_.size(); ++i)
    {
        const float distanceSquared = (particles_[i].position - point).lengthSquared();
        if (distanceSquared <= bestDistanceSquared)
        {
            bestDistanceSquared = distanceSquared;
            bestIndex = i;
        }
    }

    return bestIndex;
}

void World::setParticlePosition(std::size_t index, Vec2 position)
{
    if (index >= particles_.size())
    {
        return;
    }

    particles_[index].position = position;
    particles_[index].previousPosition = position;
}

void World::togglePinned(std::size_t index)
{
    if (index >= particles_.size())
    {
        return;
    }

    particles_[index].pinned = !particles_[index].pinned;
    particles_[index].previousPosition = particles_[index].position;
}

void World::cutConstraintsNear(Vec2 point, float radius)
{
    for (auto& constraint : constraints_)
    {
        if (!constraint.active || constraint.a >= particles_.size() || constraint.b >= particles_.size())
        {
            continue;
        }

        const float distance = distancePointToSegment(
            point,
            particles_[constraint.a].position,
            particles_[constraint.b].position);

        if (distance <= radius)
        {
            constraint.active = false;
        }
    }

    removeInactiveConstraints();
}
