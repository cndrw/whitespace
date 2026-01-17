#pragma once

// #define EDITOR_DEBUG 

#ifdef EDITOR_DEBUG
    #define DRAW_DEBUG_RECTANGLE(rect, color) \
        DrawRectangleRec(rect, color)
#else
    #define DRAW_DEBUG_RECTANGLE(rect, color) \
        ((void)0)
#endif

#include "raymath.h"

class Vec2
{
public:
    float x = 0, y = 0;

    constexpr Vec2() = default;
    constexpr Vec2(float x, float y) : x{x}, y{y} {}
    constexpr Vec2(float xy) : x{xy}, y{xy} {}
    constexpr Vec2(const Vector2& v) : x{v.x}, y{v.y} {}

    [[nodiscard]] constexpr Vec2 norm() const;
    [[nodiscard]] constexpr float len() const;
    [[nodiscard]] constexpr static float dist(Vec2 v1, Vec2 v2);
    [[nodiscard]] constexpr static float dot(Vec2 v1, Vec2 v2);
    [[nodiscard]] constexpr Vec2 operator*(float val) const;
    [[nodiscard]] constexpr Vec2& operator+=(const Vec2& other);
    [[nodiscard]] constexpr Vec2& operator-=(const Vec2& other);
    [[nodiscard]] constexpr Vec2 operator/(float val) const;
    [[nodiscard]] constexpr Vec2 operator+(const Vec2& other) const;
    [[nodiscard]] constexpr Vec2 operator-(const Vec2& rhs) const;
    [[nodiscard]] constexpr Vec2 operator-(Vec2&& rhs) const;
    [[nodiscard]] constexpr Vec2 operator-() const;
    [[nodiscard]] inline constexpr operator Vector2() const { return Vector2{ x, y }; }
    friend std::ostream& operator<<(std::ostream& stream, const Vec2& v);

};