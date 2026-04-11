#include <vector>
#include <ranges>
#include <algorithm>
#include <iostream>
#include <format>
#include <string>
#include <cstring>
#include <memory>

#include "Application.h"
#include "AppLayer.h"
#include "UIElements.h"
#include "SceneElement.h"
#include "Utils.h"

#include "raylib.h"
#include "raygui.h"

UIButton::UIButton(const Rectangle& rect, Callback on_click, const std::string& text)
    : UIButton(rect, on_click)
{
    this->text = text;
    render = [this]() { this->render_impl(); };
}

bool UIButton::is_hovered()
{
    return CheckCollisionPointRec(GetMousePosition(), rect);
}

void UIButton::render_impl()
{
    if (use_fill)
    {
        DrawRectangleRec(rect, fill_color);
    }

    GuiLabelButton(rect, text.c_str());
}

UIImageButton::UIImageButton(const Rectangle& rect, Texture2D texture, Callback on_click, Callback on_hover)
    : UIButton(rect, [](){}, on_click, on_hover), texture(texture)
{
    render = [this]() { this->render_impl(); };
}

void UIImageButton::render_impl()
{
    if (texture.id == 0)
    {
        DrawRectangleRec(rect, MAGENTA);
        return;
    }

    if (show_bg)
    {
        DrawRectangleRec(rect, bg_color);
    }
    DrawTexturePro(
        texture,
        Rectangle { 0, 0, static_cast<float>(texture.width), static_cast<float>(texture.height) },
        rect,
        Vector2 { 0, 0 },
        0.0f,
        WHITE
    );
}

UIDropDownList::UIDropDownList()
{
}

UIDropDownList::UIDropDownList(
    const Rectangle &rect_p,
    const std::string &header,
    const ExpansionType exp_type,
    const ActivationType act_type)
    : UIButton(rect_p, []() {}, []() {})
    , expansion_type{exp_type}
    , activation_type{act_type}
    , m_header(header)
{
    render = [this]() { this->render_impl(); };
    on_click = [this]() { this->on_click_impl(); };
    on_hover = [this] { on_hover_impl(); };
}

Rectangle UIDropDownList::get_new_position(const std::string& text, ExpansionType exp_type) const
{
    const float button_width = 
        MeasureTextEx(GuiGetFont(), text.c_str(), GuiGetStyle(DEFAULT, TEXT_SIZE), GuiGetStyle(DEFAULT, TEXT_SPACING)).x;

    if (exp_type == ExpansionType::DOWN)
    {
        return {
            .x = rect.x,
            .y = rect.y + rect.height * (m_items.size() + 1),
            .width = button_width,
            .height = rect.height
        };
    }
    else
    {
        return {
            .x = rect.x + rect.width,
            .y = rect.y + rect.height * m_items.size(),
            .width = button_width,
            .height = rect.height
        };
    }

}

void UIDropDownList::on_hover_impl()
{
    if (activation_type == ActivationType::HOVER)
    {
        m_state = is_hovered() ? State::OPENED : State::CLOSED;
        return;
    }


    // for (const auto& item : m_items)
    // {
    //     if (item->is_hovered())
    //     {
    //         item->on_click();
    //         m_state = State::CLOSED;
    //         return;
    //     }
    // }

    // m_state = State::CLOSED;

    for (const auto& item : m_items)
    {
        if (item->is_hovered())
        {
            item->on_hover();
            return;
        }
    }

    // if (activation_type == ActivationType::HOVER)
    // {
    //     std::cout << "hey\n";
    //     // TODO: also just quick'n dirty implemenation

    //     for (const auto& item : m_items)
    //     {
    //         if (item->is_hovered())
    //         {
    //             item->on_click();
    //             m_state = State::CLOSED;
    //             return;
    //         }
    //     }

    //     m_state = State::CLOSED;
    // }
}

void UIDropDownList::render_impl()
{
    if (m_state == State::CLOSED)
    {
        GuiLabelButton(rect, m_header.c_str());
    }
    else if (!m_items.empty())
    {
        const auto width = (*std::ranges::max_element(m_items, [](const auto& a, const auto& b) {
                return a->rect.width < b->rect.width;
            }))->rect.width;

        const Rectangle scene_list_rect = {
            rect.x + rect.width * (expansion_type == ExpansionType::SIDE),
            rect.y + rect.height * (expansion_type == ExpansionType::DOWN),
            width,
            rect.height * m_items.size()
        };

        DrawRectangleRec(scene_list_rect, LIGHTGRAY); 
        GuiLabelButton(rect, m_header.c_str());

        for (const auto& item : m_items)
        {
            item->render();
        }
    }
}

void UIDropDownList::on_click_impl()
{
    if (m_header_hovered)
    {
        m_state = (m_state == State::CLOSED) ? State::OPENED : State::CLOSED;
        return;
    }

    for (const auto& item : m_items)
    {
        if (item->is_hovered())
        {
            item->on_click();
            m_state = State::CLOSED;
            return;
        }
    }

    m_state = State::CLOSED;
}

void UIDropDownList::add_item(const std::string& item, Callback on_click)
{
    Rectangle button_rect = get_new_position(item, expansion_type);

    m_items.push_back(
        std::make_unique<UIButton>(
            button_rect,
            [this, button_rect, item]() {
                GuiLabelButton(button_rect, item.c_str());
            },
            on_click
        )
    );

    m_items.back()->text = item;
}

UIDropDownList& UIDropDownList::add_nested(const std::string& name, const ExpansionType exp_type)
{

    auto drop_down_menu = std::make_unique<UIDropDownList>(
        get_new_position(name, expansion_type),
        name,
        exp_type,
        ActivationType::HOVER
    );

    UIDropDownList& ref = *drop_down_menu;
    ref.text = "nested";

    m_items.push_back(std::move(drop_down_menu));
    m_items.back()->on_hover();

    return ref;
}

bool UIDropDownList::is_hovered()
{
    Vector2 mouse_pos = GetMousePosition();
    m_header_hovered = false;

    if (m_state == State::CLOSED)
    {
        m_header_hovered = CheckCollisionPointRec(mouse_pos, rect);
        return m_header_hovered;
    } 
    else 
    {
        if (CheckCollisionPointRec(mouse_pos, rect))
        {
            m_header_hovered = true;
            return true;
        }

        for (const auto& item : m_items)
        {
            if (item->is_hovered())
                return true;
        }

        // this is more of a workaround since currently if i press somewhere else
        // on_click() wont get triggered because it is not hovered over...
        if (IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_LEFT))
        {
            m_state = State::CLOSED;
        }

        if (activation_type == ActivationType::HOVER)
        {
            m_state = State::CLOSED;
        }

        return false;
    }
}

void UIComponent::render()
{
    DrawRectangleRec(m_outer_rect, m_bg_color);
    GuiGroupBox(m_inner_rect, m_name.c_str());

    render_impl();
}

void UIComponent::set_rect(const Rectangle rect)
{
    constexpr float padding { 6.0 };
    m_outer_rect = rect;
    m_inner_rect = {
        .x = rect.x + padding,
        .y = rect.y + padding,
        .width = rect.width - 2 * padding,
        .height = rect.height - 2 * padding
    };
}

UITextBox::UITextBox(const size_t buffer_size)
    : on_edited{[](){}}, m_buffer_size{buffer_size}
{
    render = [this] { render_impl(); };
    m_buffer = new char[m_buffer_size];
    std::memset(m_buffer, 0, m_buffer_size);
}

UITextBox::~UITextBox()
{
    delete[] m_buffer;
}

// TODO: UITextbox sollte wissen ob sich der content geändert hat
//       -> muss nicht on_edited() callen
bool UITextBox::process_input()
{
    bool click_processed = false;
    if (IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_LEFT))
    {
        if (is_hovered())
        {
            on_click();
            click_processed = true;
        }
        else if (m_edit_mode)
        {
            m_edit_mode = false;
            on_edited();
        }
    }

    if (m_edit_mode && IsKeyPressed(KEY_V) && IsKeyDown(KEY_LEFT_CONTROL))
    {
        set_text(GetClipboardText());
        return true;
    }

    // enter is not "processed", so canvas layer can update the sprite immediently
    if (IsKeyPressed(KEY_ENTER))
    {
        m_edit_mode = false;
        on_edited();
    }
    
    return click_processed;
}

void UITextBox::on_click()
{
    m_edit_mode = true;
}

void UITextBox::cancel_edit()
{
    m_edit_mode = false;
}

bool UITextBox::is_hovered()
{
    return CheckCollisionPointRec(GetMousePosition(), rect);
}

void UITextBox::set_text(const std::string& text)
{
    if (text.size() > m_buffer_size)
    {
        std::cerr << std::format("[ERROR] UITextBox string given with 'set_text' exceeds the buffer size of {}\n", m_buffer_size);
        return;
    }

    std::memset(m_buffer, 0, m_buffer_size);

    for (std::size_t i = 0; i < text.size(); i++)
    {
        m_buffer[i] = text[i];
    }
}

void UITextBox::set_static(bool status)
{
    m_draw_label = status;
}

void UITextBox::clear()
{
    std::memset(m_buffer, 0, m_buffer_size);
}

void UITextBox::render_impl()
{
    if (m_draw_label)
    {
        GuiLabel(rect, m_buffer);
        return;
    }

    DRAW_DEBUG_RECTANGLE(rect, BLUE);
    GuiTextBox(rect, m_buffer, m_buffer_size, m_edit_mode);
}

UIScrollView::UIScrollView()
{
    // yuck... abomination (TODO) -> render_impl should be pure virtual from UIElement
    render = [this] { render_impl(); };
}

Rectangle UIScrollView::get_new_rect() const
{
    static constexpr auto padding_between   { 5 };
    static constexpr auto padding_left      { 5 };
    static constexpr auto height            { 15 };

    if (m_entries.empty())
    {
        return {
            rect.x + padding_left, rect.y + padding_between + 2,
            rect.width, height
        };
    }

    Rectangle last_rect = m_entries.back().button->rect;
    last_rect.y += last_rect.height;

    return last_rect;
}

void UIScrollView::add_entry(const SpriteElement& elem, Callback on_click)
{
    m_entries.push_back({
        .active = true, 
        .button = std::make_unique<UIButton>(
            get_new_rect(),
            on_click,
            elem.name
    )});

    m_entries.back().active = check_if_in_view(m_entries.back());
}

void UIScrollView::remove_entry(const SpriteElement& elem)
{
    auto start = std::ranges::find_if(m_entries, [&elem] (auto& e) { return elem.name == e.button->text; });

    if (start == m_entries.end())
    {
        return;
    }

    for (auto it = std::prev(m_entries.end()); it != start; it--)
    {
        auto prev = std::prev(it);
        it->button->rect.y = prev->button->rect.y;
    }

    m_entries.erase(start);
}

void UIScrollView::update_entry(const std::string_view before, const std::string_view after)
{
    for (auto& [_, button] : m_entries)
    {
        if (button->text == before)
        {
            button->text = after;
            return;
        }
    }
}

void UIScrollView::render_impl() const
{
    DRAW_DEBUG_RECTANGLE(rect, BLUE);

    for (const auto& [active, button] : m_entries)
    {
        if (active)
        {
            button->render();
        }
    }
}

bool UIScrollView::process_input()
{
    handle_scrolling();

    for (const auto& [active, button] : m_entries)
    {
        if (active && button->is_hovered())
        {
            if (IsMouseButtonPressed(MouseButton::MOUSE_BUTTON_LEFT))
            {
                button->on_click();
                return true;
            }
        }
    }

    return CheckCollisionPointRec(GetMousePosition(), rect);
}

void UIScrollView::handle_scrolling()
{
    static constexpr auto SCROLL_SPEED { 10 };
    float scroll_y = 0;

    if (CheckCollisionPointRec(GetMousePosition(), rect))
    {
        scroll_y = GetMouseWheelMoveV().y;
    }

    if (!m_entries.empty())
    {
        int8_t sign = 0;
        if (scroll_y > 0 && m_entries.front().button->rect.y < rect.y)
        {
            sign = 1;
        }
        else if (scroll_y < 0 && m_entries.back().button->rect.y + m_entries.back().button->rect.height > rect.y + rect.height)
        {
            sign = -1;
        }

        for (auto& entry : m_entries)
        {
            entry.button->rect.y += sign * SCROLL_SPEED;
            entry.active = check_if_in_view(entry);
        }
    }
}

bool UIScrollView::check_if_in_view(const Entry& entry) const
{
    auto& r = entry.button->rect;
    return r.y + r.height <= rect.y + rect.height && r.y > rect.y;
}

void UIScrollView::clear()
{
    m_entries.clear();
}

UIWindowBase::UIWindowBase(const Rectangle& rect, const char* title)
    : m_title{title} 
{
    this->rect = rect;

    // TODO: see above
    render = [this] { render_impl(); };
    m_content_rect = {
        .x = rect.x + 50,
        .y = rect.y + 50,
        .width = rect.width - 50 * 2,
        .height = rect.height - 50 * 2
    };
}

void UIWindowBase::open()
{
    m_state = State::OPEN;
}

void UIWindowBase::close()
{
    m_state = State::CLOSED;
    on_close();
}

bool UIWindowBase::is_open() const
{
    return m_state == State::OPEN;
}

void UIWindowBase::render_impl()
{
    if (m_state == State::OPEN)
    {
        // i know... but could'nt be bothered
        if (GuiWindowBox(rect, m_title))
        {
            close();
        }

        DRAW_DEBUG_RECTANGLE(m_content_rect, BLUE);
        render_content();
    }
}

OpenProjectWindow::OpenProjectWindow(const Rectangle& rect)
    : UIWindowBase(rect, "Open Project")
{
    m_textbox = std::make_unique<UITextBox>(150);
    auto label_rect = m_content_rect;
    label_rect.y += 20;
    label_rect.height = 30;
    m_textbox->rect = label_rect;

    label_rect.y += 40;
    label_rect.width = 60;

    m_open_button = std::make_unique<UIButton>(label_rect, [this] {
        std::cout << std::format("Open Project {}\n", m_textbox->get_text());

        if (const auto res = Core::Application::get().get_layer<AppLayer>()->open_project(m_textbox->get_text()); res.first)
        {
            close();
        }
        else
        {
            m_erro_msg = res.second;
        }
    });

    // TODO: too lazy to rewirte the UIButton... (normally this should use GuiButton, but current
    // implementation uses GuiLabel...)
    m_open_button->render = [rect = m_open_button->rect] {
        GuiButton(rect, "Open");
    };
}

bool OpenProjectWindow::process_input()
{
    if (!is_open()) return false; 

    if (m_open_button->is_hovered() && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        m_open_button->on_click();
        return true;
    }

    return m_textbox->process_input() || is_open();
}

void OpenProjectWindow::render_content()
{
    auto rect = m_textbox->rect;
    rect.y -= 23;

    GuiLabel(rect, "Enter path to project file (e.g. game.wsproj)");
    m_textbox->render();
    m_open_button->render();

    rect = m_open_button->rect;
    rect.x += rect.width + 20;
    rect.y += 10;
    DrawText(m_erro_msg.c_str(), rect.x, rect.y, 14, RED);
}

void OpenProjectWindow::on_close()
{
    m_erro_msg.clear();
    m_textbox->clear();
}

CreateProjectWindow::CreateProjectWindow(const Rectangle &rect)
    : UIWindowBase(rect, "Create Project")
{
    m_dir_textbox = std::make_unique<UITextBox>(150);
    auto label_rect = m_content_rect;
    label_rect.y += 20;
    label_rect.height = 30;
    m_dir_textbox->rect = label_rect;

    m_name_textbox = std::make_unique<UITextBox>(50);
    label_rect.y += label_rect.height + 30;
    m_name_textbox->rect = label_rect;

    label_rect.y += 40;
    label_rect.width = 60;

    m_create_button = std::make_unique<UIButton>(label_rect, [this] {
        auto file_path = (std::filesystem::path(m_dir_textbox->get_text()) / m_name_textbox->get_text());
        file_path.replace_extension(".wsproj");

        if (const auto res = Core::Application::get().get_layer<AppLayer>()->create_project(file_path); res.first)
        {
            close();
        }
        else
        {
            m_erro_msg = res.second;
        }
    });

    // TODO: too lazy to rewirte the UIButton... (normally this should use GuiButton, but current
    // implementation uses GuiLabel...)
    m_create_button->render = [rect = m_create_button->rect] {
        GuiButton(rect, "Create");
    };

}

bool CreateProjectWindow::process_input()
{
    if (!is_open()) return false; 

    if (m_create_button->is_hovered() && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    { 
        m_create_button->on_click();
        return true;
    }
    bool processed = m_name_textbox->process_input();
    processed = m_dir_textbox->process_input() || processed;

    return  processed || is_open();
}

void CreateProjectWindow::render_content()
{
    DRAW_DEBUG_RECTANGLE(m_content_rect, MAGENTA);

    auto rect = m_dir_textbox->rect;
    rect.y -= 23;
    GuiLabel(rect, "Enter directory path to save to:");
    m_dir_textbox->render();

    rect = m_name_textbox->rect;
    rect.y -= 23;
    DRAW_DEBUG_RECTANGLE(rect, YELLOW);
    GuiLabel(rect, "Enter name:");
    m_name_textbox->render();

    m_create_button->render();

    rect = m_create_button->rect;
    rect.x += rect.width + 20;
    rect.y += 10;
    DrawText(m_erro_msg.c_str(), rect.x, rect.y, 14, RED);
}

void CreateProjectWindow::on_close()
{
    m_erro_msg.clear();
    m_dir_textbox->clear();
    m_name_textbox->clear();
}
