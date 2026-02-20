#include <format>
#include <iostream>

#include "raygui.h"

#include "Inspector.h"
#include "SceneElement.h"
#include "UIElements.h"
// #define EDITOR_DEBUG
#include "Utils.h"



Inspector::Inspector()
    : UIComponent("Inspector")
{
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

    // render edit labels
    for (uint8_t i = 0; i < m_label_count; i++)
    {
        const Rectangle rect = {
            m_labels[i].pos.x, m_labels[i].pos.y,
            40, (float)m_label_height
        };
        GuiLabel(rect, m_labels[i].label.c_str());
        DRAW_DEBUG_RECTANGLE(rect, MAGENTA);
        m_labels[i].text_box->render();
    }

    // preview the asset texture (if asset is focused)
    if (m_current_disp_t == DisplayType::AssetElement)
    {
        const float width = m_inner_rect.width - 20.0f;
        const Vec2 pos = { m_inner_rect.x + 10.0f, m_inner_rect.y + m_inner_rect.height - width - 10.0f };
        const Rectangle asset_rect = { pos.x, pos.y, width, width };

        DrawTexturePro(
            m_focused_asset.texture,
            { 0.0f, 0.0f, (float)m_focused_asset.texture.width, (float)m_focused_asset.texture.height },
            asset_rect,
            { 0.0f, 0.0f },
            0.0f,
            WHITE
        );
    }
}

void Inspector::setup_labels(DisplayType disp_t)
{
    switch (disp_t)
    {
        case DisplayType::SpriteElement:
        {
            set_label("Name:", 0, m_focused_sprite.name);
            set_label("Y:", 1, m_focused_sprite.pos.x);
            set_label("X:", 2, m_focused_sprite.pos.y);
            set_label("Angle:", 3, m_focused_sprite.angle);
            set_label("Layer:", 4, m_focused_sprite.layer);
            m_label_count = 5;
            break;
        }
        case DisplayType::AssetElement:
        {
            set_label("Name:", 0, m_focused_asset.name);
            set_label("PPU:", 1, m_focused_asset.ppu);
            m_label_count = 2;
            break;
        }
        default:
            break;
    }

    // TODO: maybe find a better place for this
    m_label_width = m_outer_rect.width - 20; // 10 padding each side
    m_label_height = 15; 

    // positioning the labels
    static constexpr auto spacing { 15 };
    static constexpr auto padding { 10 };
    static constexpr auto tb_padding_right { 5 };

    for (uint8_t i = 0; i < m_label_count; i++)
    {
        Vec2 pos = { m_inner_rect.x + padding, m_inner_rect.y + padding + i * 20 };
        m_labels[i].pos = pos;

        const float label_width = MeasureTextEx(GuiGetFont(), m_labels[i].label.c_str(), GuiGetStyle(DEFAULT, TEXT_SIZE), GuiGetStyle(DEFAULT, TEXT_SPACING)).x;
        const float tb_x = pos.x + label_width + spacing;

        m_labels[i].text_box->rect = {
            tb_x, pos.y,
            m_inner_rect.x + m_inner_rect.width - tb_x - tb_padding_right, (float)m_label_height
        };
    }
}

void Inspector::update_content(const std::optional<SpriteElement>& element)
{
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
    setup_labels(m_current_disp_t);
}
