#pragma once

#include <optional>
#include <functional>

#include "raymath.h"

#include "SceneElement.h"
#include "AssetManager.h"
#include "UIElements.h"
#include "Utils.h"

typedef struct 
{
    std::string label;
    std::unique_ptr<UITextBox> text_box;
    Vec2 pos;
} LabeledTextBox;


class Inspector : public UIComponent
{
public:
    Inspector();
    bool process_input();
    void update_content(const std::optional<SpriteElement>& element); 
    void update_content(const Core::Asset& element); 
    Rectangle get_rect() const { return m_outer_rect; }

protected:
    void render_impl() override;

private:
    enum class DisplayType { None = -1, SceneElement, SpriteElement, AssetElement };
    void setup_labels(DisplayType disp_t);
    void draw_label(const Vec2 pos, const std::string label) const;
    void draw_scene_element_content() const;
    void draw_sprite_element_content() const;
    void draw_asset_element_content() const;

private:
    int m_label_width, m_label_height;
    std::array<std::function<void()>, 3> m_draw_funcs;
    std::array<LabeledTextBox ,5> m_labels;
    DisplayType m_current_disp_t = DisplayType::None;
    SpriteElement m_focused_sprite;
    Core::Asset m_focused_asset;
};