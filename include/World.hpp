#pragma once

#include "Vec2.hpp"

#include <cstddef>
#include <optional>
#include <vector>

struct Particle
{
    Vec2 position;
    Vec2 previousPosition;
    Vec2 acceleration;
    float radius = 6.0f;
    bool pinned = false;

    Particle(Vec2 start, float particleRadius, bool isPinned)
        : position(start), previousPosition(start), radius(particleRadius), pinned(isPinned)
    {
    }
};

struct Constraint
{
    std::size_t a = 0;
    std::size_t b = 0;
    float restLength = 0.0f;
    float stiffness = 1.0f;
    float tearLength = 0.0f;
    bool active = true;

    Constraint(std::size_t first, std::size_t second, float length, float stiff, float tear)
        : a(first), b(second), restLength(length), stiffness(stiff), tearLength(tear)
    {
    }
};

class World
{
public:
    World(float width, float height);

    std::size_t addParticle(Vec2 position, float radius = 6.0f, bool pinned = false);
    void addConstraint(std::size_t a, std::size_t b, float stiffness = 1.0f, float tearMultiplier = 2.2f);

    void step(float dt, float timeSeconds);
    void clear();
    void createDemoScene();

    void createRope(Vec2 start, int segments, float segmentLength, bool pinStart);
    void createCloth(Vec2 topLeft, int columns, int rows, float spacing);
    void createSoftBody(Vec2 center, float radius, int points);

    std::optional<std::size_t> findNearestParticle(Vec2 point, float maxDistance) const;
    void setParticlePosition(std::size_t index, Vec2 position);
    void togglePinned(std::size_t index);
    void cutConstraintsNear(Vec2 point, float radius);

    const std::vector<Particle>& particles() const { return particles_; }
    const std::vector<Constraint>& constraints() const { return constraints_; }
    std::vector<Particle>& editableParticles() { return particles_; }

    float width() const { return width_; }
    float height() const { return height_; }

    bool windEnabled = true;
    bool tearingEnabled = true;
    bool gravityEnabled = true;
    int constraintIterations = 8;

private:
    void integrate(float dt, float timeSeconds);
    void solveConstraints();
    void keepInsideBounds();
    void removeInactiveConstraints();

    std::vector<Particle> particles_;
    std::vector<Constraint> constraints_;
    float width_ = 0.0f;
    float height_ = 0.0f;
    Vec2 gravity_ = {0.0f, 1250.0f};
    float drag_ = 0.995f;
    float bounce_ = 0.25f;
};
