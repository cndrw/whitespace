#pragma once

#include <vector>

#include "raylib.h"

#include "UIElements.h"
#include "AppLayer.h"

class FunctionRibbon
{
public:
    FunctionRibbon();
    bool process_input();
    void render();
    UIElement make_button(const Rectangle& rect, const char* label, const std::function<void()>& on_click);
    void update_ribbon(const ProjectMetadata& proj_data);
    void set_rect(Rectangle rect);

private:
    Rectangle m_rect;
    std::vector<UIElement> m_buttons; 
};