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
    [[nodiscard]] constexpr Vec2 operator*(float val) const
    {
        return {x * val, y * val};
    }

    constexpr Vec2& operator+=(const Vec2& other)
    {
        this->x += other.x;
        this->y += other.y;
        return *this;
    }

    [[nodiscard]] constexpr Vec2& operator-=(const Vec2& other);

    [[nodiscard]] constexpr Vec2 operator/(double val) const
    {
        if (val == 0)
        {
            throw std::invalid_argument("Can't divide Vec2 by zero.");
        }

        Vec2 res = *this;
        res.x /= val;
        res.y /= val;
        return res;
    }

    [[nodiscard]] constexpr Vec2 operator/(float val) const
    {
        return *this / static_cast<double>(val);
    }
     
    [[nodiscard]] constexpr Vec2 operator+(const Vec2& other) const
    {
        return { x + other.x, y + other.y };
    }

    [[nodiscard]] constexpr Vec2 operator+(Vec2&& other) const
    {
        return { x + other.x, y + other.y };
    }

    [[nodiscard]] constexpr Vec2 operator-(const Vec2& other) const
    {
        return { x - other.x, y - other.y };
    }

    [[nodiscard]] constexpr Vec2 operator-(Vec2&& other) const
    {
        return {x - other.x, y - other.y};
    }

    [[nodiscard]] constexpr Vec2 operator-() const
    {
        return {-this->x, -this->y};
    }
    [[nodiscard]] inline constexpr operator Vector2() const { return Vector2{ x, y }; }
    friend std::ostream& operator<<(std::ostream& stream, const Vec2& v);

};


[[nodiscard]] bool IsMouseButtonPressedTwice(int button);


class Res
{
public:
    Res(const bool result, const std::string& msg) : msg{msg}, m_result{result} {}

    inline operator bool() const { return m_result; }

    std::string msg;

private:
    bool m_result;
};
