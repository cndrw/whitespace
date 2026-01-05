#pragma once

#include <functional>

#include "raylib.h"

typedef struct {
    Rectangle rect;
    std::function<void()> on_click;
    std::function<void()> render;
} UIElement;