#include <math.h>
#include <stdexcept>
#include <ostream>
#include <format>

#include "Application.h"
#include "Utils.h"

[[nodiscard]]
constexpr Vec2& Vec2::operator+=(const Vec2& other)
{
    this->x += other.x;
    this->y += other.y;
    return *this;
}

[[nodiscard]]
constexpr Vec2 &Vec2::operator-=(const Vec2& other)
{
    this->x -= other.x;
    this->y -= other.y;
    return *this;
}

[[nodiscard]]
constexpr Vec2 Vec2::operator-() const
{
    return {-this->x, -this->y};
}

[[nodiscard]]
constexpr Vec2 Vec2::operator/(const float val) const
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

[[nodiscard]]
constexpr Vec2 Vec2::operator*(const float val) const
{
    Vec2 res = *this;
    res.x *= val;
    res.y *= val;
    return res;
}

[[nodiscard]]
constexpr Vec2 Vec2::operator+(const Vec2& other) const
{
    Vec2 res = *this;
    res.x += other.x;
    res.y += other.y;
    return res;
}

[[nodiscard]]
constexpr Vec2 Vec2::operator-(const Vec2& other) const
{
    Vec2 res = *this;
    res.x -= other.x;
    res.y -= other.y;
    return res;
}

[[nodiscard]]
constexpr Vec2 Vec2::operator-(Vec2&& rhs) const
{
    Vec2 res = *this;
    res.x -= rhs.x;
    res.y -= rhs.y;
    return res;
}

std::ostream& operator<<(std::ostream& stream, const Vec2& v)
{
    stream << "x: " << v.x << " y: " << v.y << " ";
    return stream;
}

[[nodiscard]]
constexpr float Vec2::dist(const Vec2 v1, const Vec2 v2)
{
    const Vec2 v_distance  = v2 - v1;
    return v_distance.len();
}

[[nodiscard]]
constexpr Vec2 Vec2::norm() const
{
    const auto l = len();
    return {x / l, y / l};
}

[[nodiscard]]
constexpr float Vec2::len() const
{
    return std::sqrt(x * x + y * y);
}

[[nodiscard]]
constexpr float Vec2::dot(const Vec2 v1, const Vec2 v2)
{
    return v1.x * v2.x + v1.y * v2.y;
}

[[nodiscard]]
bool IsMouseButtonPressedTwice(int button)
{
    static float last_click = 0;
    constexpr float max_delay = 0.3f;

    if (std::fabs(last_click - Core::Application::get_time()) < 0.05)
    {
        // already processed this frame
        return false;
    }

    if (IsMouseButtonPressed(button))
    {
        const float current_time = Core::Application::get_time(); 
        const float abs_diff = std::fabs(current_time - last_click);

        // small hack so that checking this mutlitple times in the same frame 
        // does not cause problems 
        if (abs_diff < 0.01) return false; 

        if (abs_diff < max_delay)
        {
            last_click = 0;
            return true;
        }
        last_click = current_time;
    }
    return false;
}
