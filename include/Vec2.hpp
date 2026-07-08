#pragma once

#include <cmath>

struct Vec2
{
    float x = 0.0f;
    float y = 0.0f;

    Vec2() = default;
    Vec2(float xValue, float yValue) : x(xValue), y(yValue) {}

    Vec2 operator+(const Vec2& other) const { return {x + other.x, y + other.y}; }
    Vec2 operator-(const Vec2& other) const { return {x - other.x, y - other.y}; }
    Vec2 operator*(float value) const { return {x * value, y * value}; }
    Vec2 operator/(float value) const { return {x / value, y / value}; }

    Vec2& operator+=(const Vec2& other)
    {
        x += other.x;
        y += other.y;
        return *this;
    }

    Vec2& operator-=(const Vec2& other)
    {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    Vec2& operator*=(float value)
    {
        x *= value;
        y *= value;
        return *this;
    }

    float lengthSquared() const { return x * x + y * y; }

    float length() const { return std::sqrt(lengthSquared()); }

    Vec2 normalized() const
    {
        const float len = length();
        if (len <= 0.0001f)
        {
            return {0.0f, 0.0f};
        }
        return *this / len;
    }
};

inline Vec2 operator*(float value, const Vec2& v)
{
    return {v.x * value, v.y * value};
}
