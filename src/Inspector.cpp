#include <format>
#include <iostream>

#include "raygui.h"

#include "Inspector.h"
#include "SceneElement.h"
#include "UIElements.h"
// #define EDITOR_DEBUG
#include "Utils.h"


int to_draw_idx = 0;

Inspector::Inspector()
    : UIComponent("Inspector")
{
    m_draw_funcs[(int)DisplayType::SceneElement] = [this] () { draw_scene_element_content(); };
    m_draw_funcs[(int)DisplayType::SpriteElement] = [this] () { draw_sprite_element_content(); };
    m_draw_funcs[(int)DisplayType::AssetElement] = [this] () { draw_asset_element_content(); };

    for (auto& label : m_labels)
    {
        label = { "", std::make_unique<UITextBox>(), Vec2{0, 0}, [](){} };
    }
}

bool Inspector::process_input()
{
    bool click_processed = false;
    for (const auto& label : m_labels)
    {
        if (IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_LEFT))
        {
            if (label.text_box->is_hovered())
            {
                label.text_box->on_click();
                click_processed = true;
            }
            else 
            {
                label.text_box->cancel_edit();
                label.on_edited();
            }
        }

        if (IsKeyPressed(KEY_ENTER))
        {
            label.text_box->cancel_edit();
            label.on_edited();
        }
    }

    return CheckCollisionPointRec(GetMousePosition(), m_outer_rect) || click_processed;
}

void Inspector::render_impl()
{
    if (m_current_disp_t == DisplayType::None) { return; }

    m_draw_funcs[static_cast<int>(m_current_disp_t)]();
}


void Inspector::setup_labels(DisplayType disp_t)
{
    switch (disp_t)
    {
        case DisplayType::SpriteElement:
        {
            m_labels[0].label = "Name:";
            m_labels[0].text_box->set_text(m_focused_sprite.name);
            m_labels[0].on_edited = [this] { handle_edit(m_focused_sprite.name, 0); };

            m_labels[1].label = "X:";
            m_labels[1].text_box->set_text(std::to_string(m_focused_sprite.pos.x));
            m_labels[1].on_edited = [this] { handle_edit(m_focused_sprite.pos.x, 1); };

            m_labels[2].label = "Y:";
            m_labels[2].text_box->set_text(std::to_string(m_focused_sprite.pos.y));
            m_labels[2].on_edited = [this] { handle_edit(m_focused_sprite.pos.y, 2); };

            m_labels[3].label = "Angle:";
            m_labels[3].text_box->set_text(std::to_string(m_focused_sprite.angle));
            m_labels[3].on_edited = [this] { handle_edit(m_focused_sprite.angle, 3); };

            m_labels[4].label = "Layer:";
            m_labels[4].text_box->set_text(std::to_string(m_focused_sprite.layer));
            m_labels[4].on_edited = [this] { handle_edit(m_focused_sprite.layer, 4); };
            to_draw_idx = 5;
            break;
        }
        case DisplayType::AssetElement:
        {
            m_labels[0].label = "Name:";
            m_labels[0].text_box->set_text(m_focused_asset.path.stem().string());

            m_labels[1].label = "PPU:";
            m_labels[1].text_box->set_text(std::to_string(m_focused_asset.ppu));
            to_draw_idx = 2;
            break;
        }
        default:
            break;
    }

    static constexpr auto spacing { 15 };
    static constexpr auto tb_padding_right { 5 };

    for (int i = 0; i < to_draw_idx; i++)
    {
        Vec2 pos = { m_inner_rect.x + 10, m_inner_rect.y + 10 + i * 20 };
        m_labels[i].pos = pos;

        const float label_width = MeasureTextEx(GuiGetFont(), m_labels[i].label.c_str(), GuiGetStyle(DEFAULT, TEXT_SIZE), GuiGetStyle(DEFAULT, TEXT_SPACING)).x;
        const float tb_x = pos.x + label_width + spacing;

        m_labels[i].text_box->rect = {
            tb_x, pos.y,
            m_inner_rect.x + m_inner_rect.width - tb_x - tb_padding_right, (float)m_label_height
        };
    }
}

void Inspector::draw_label(Vec2 pos, std::string text) const
{
    // draw relative to inspector pos
    const auto label_pos =  Vec2{ m_inner_rect.x, m_inner_rect.y } + pos;
    const Rectangle label_rect =
        { label_pos.x, label_pos.y, (float)m_label_width, (float)m_label_height };
    DRAW_DEBUG_RECTANGLE(label_rect, MAGENTA);
    GuiLabel(label_rect, text.c_str());
}

void Inspector::draw_scene_element_content() const
{
    std::cout << "draw_scene_element_content not yet implemented!\n";
}

void Inspector::draw_sprite_element_content() const
{
    for (int i = 0; i < to_draw_idx; i++)
    {
        const Rectangle rect = {
            m_labels[i].pos.x, m_labels[i].pos.y,
            40, (float)m_label_height
        };
        GuiLabel(rect, m_labels[i].label.c_str());
        DRAW_DEBUG_RECTANGLE(rect, MAGENTA);
        m_labels[i].text_box->render();
    }
}

void Inspector::draw_asset_element_content() const
{
    draw_label({10, 10}, std::format("Name: {}", m_focused_asset.path.stem().string().c_str()));
    draw_label({10, 30}, std::format("PPU: {}", m_focused_asset.ppu));

    const float width = m_inner_rect.width - 20.0f;
    const Vec2 pos = { m_inner_rect.x + 10.0f, m_inner_rect.y + m_inner_rect.height - width - 10.0f };
    const Rectangle asset_rect = { pos.x, pos.y, width, width };


    DRAW_DEBUG_RECTANGLE(asset_rect, MAGENTA);
    DrawTexturePro(
        m_focused_asset.texture,
        { 0.0f, 0.0f, (float)m_focused_asset.texture.width, (float)m_focused_asset.texture.height },
        asset_rect,
        { 0.0f, 0.0f },
        0.0f,
        WHITE
    );
}


void Inspector::update_content(const std::optional<SpriteElement>& element)
{
    // TODO: maybe find a better place for this
    m_label_width = m_outer_rect.width - 20; // 10 padding each side
    m_label_height = 15; 

    m_current_disp_t = element.has_value() ? DisplayType::SpriteElement : DisplayType::None;
    if (m_current_disp_t == DisplayType::SpriteElement)
    {
        m_focused_sprite = element.value();
        setup_labels(m_current_disp_t);
    }
}

void Inspector::update_content(const Core::Asset& element)
{
    m_current_disp_t = DisplayType::AssetElement;
    m_focused_asset = element;
}
