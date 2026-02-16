#pragma once

#include <optional>
#include <format>
#include <functional>

#include "raymath.h"

#include "SceneElement.h"
#include "AssetManager.h"
#include "UIElements.h"
#include "Utils.h"
#include "Event.h"

typedef struct 
{
    std::string label;
    std::unique_ptr<UITextBox> text_box;
    Vec2 pos;
    std::function<void()> on_edited;
} LabeledTextBox;


class Inspector : public UIComponent
{
public:
    Inspector();
    bool process_input();
    void update_content(const std::optional<SpriteElement>& element); 
    void update_content(const Core::Asset& element); 
    Rectangle get_rect() const { return m_outer_rect; }

    Core::Event<SpriteElement> on_sprite_elem_changed;

protected:
    void render_impl() override;

private:
    enum class DisplayType { None = -1, SceneElement, SpriteElement, AssetElement };
    void setup_labels(DisplayType disp_t);
    void draw_label(const Vec2 pos, const std::string label) const;
    void draw_scene_element_content() const;
    void draw_sprite_element_content() const;
    void draw_asset_element_content() const;

    template<typename T>
    void handle_edit(T& field, const uint8_t idx)
    {
        try {
            field = parse_textbox<T>(m_labels[idx].text_box.get());
            on_sprite_elem_changed.invoke(m_focused_sprite);
        }
        catch (...) { }
    }

    template<typename T>
    T parse_textbox(const UITextBox* textbox) const
    {
        std::string txt = textbox->get_text();
        if constexpr (std::is_same_v<T, std::string>)
        {
            return txt;
        }
        else
        {
            // assume number edit labels
            size_t pos;
            float new_value = std::stof(txt, &pos);;

            if (pos != txt.size())
            {
                throw std::invalid_argument("extra characters");
            }
            return new_value;
        }
    }

private:
    int m_label_width, m_label_height;
    std::array<std::function<void()>, 3> m_draw_funcs;
    std::array<LabeledTextBox ,5> m_labels;
    DisplayType m_current_disp_t = DisplayType::None;
    SpriteElement m_focused_sprite;
    Core::Asset m_focused_asset;
};