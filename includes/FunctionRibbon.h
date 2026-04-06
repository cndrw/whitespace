#pragma once

#include <vector>

#include "raylib.h"

#include "UIElements.h"
#include "AppLayer.h"

class FunctionRibbon : public UIComponent
{
public:
    FunctionRibbon();
    ~FunctionRibbon();
    bool process_input();
    void render() override;
    UIButton* make_button(const Rectangle& rect, const char* label, const std::function<void()>& on_click);
    void update_ribbon(const ProjectMetadata& proj_data);

// protected:
//     void render_impl() override;

private:
    Rectangle m_scene_static_label;
    std::vector<UIButton*> m_buttons; 
    std::vector<UIElement*> m_labels;
    std::unique_ptr<OpenProjectWindow> m_open_project_window;
    std::unique_ptr<CreateProjectWindow> m_create_project_window;
};