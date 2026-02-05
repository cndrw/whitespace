#pragma once

#include <functional>

#include "raylib.h"


class UIElement
{
public:
    using Callback = std::function<void()>;
    UIElement() = default;

    UIElement(const Rectangle& rect)
        : rect(rect), render([](){}) {}

    UIElement(const Rectangle& rect, Callback render)
        : rect(rect), render(render) {}

    virtual ~UIElement() = default;

    virtual bool is_hovered() { return true; }

public:
    Rectangle rect;
    Callback render;
};

class UIButton : public UIElement
{
public:
    UIButton() = default;

    UIButton(const Rectangle& rect, Callback on_click, const std::string& text);

    UIButton(const Rectangle& rect, Callback on_click)
        : UIButton(rect, [](){}, on_click) {}

    UIButton(const Rectangle& rect, Callback render, Callback on_click)
        : UIButton(rect, render, on_click, [](){}) {}

    UIButton(const Rectangle& rect, Callback render, Callback on_click, Callback on_hover)
        : UIElement(rect, render), on_click(on_click), on_hover(on_hover) {}

    virtual ~UIButton() = default;

    virtual bool is_hovered() override;

private:
    virtual void render_impl();

public:
    Callback on_click;
    Callback on_hover;
    std::string text;
};

class UIImageButton : public UIButton
{
public:
    UIImageButton() = default;

    UIImageButton(Texture2D texture, Callback on_click)
        : UIImageButton(texture, on_click, [](){}) {}

    UIImageButton(Texture2D texture, Callback on_click, Callback on_hover)
        : UIImageButton(Rectangle{}, texture, on_click, on_hover) {}

    UIImageButton(const Rectangle& rect, Texture2D texture, Callback on_click, Callback on_hover);

    virtual ~UIImageButton() = default;

private:
    virtual void render_impl() override;

public:
    Texture2D texture;
    Color bg_color = { 170, 87, 76, 200 };
    bool show_bg = false;
};

class UIDropDownList : public UIButton 
{
public:
    UIDropDownList() = default;

    UIDropDownList(const Rectangle& rect_p, const std::string& header);

    virtual ~UIDropDownList() = default;

    inline void clear_items() { m_items.clear(); }
    void add_item(const std::string& item, Callback on_click);
    inline void set_header(const std::string& header) { m_header = header; }
    virtual bool is_hovered() override;

private:
    void render_impl();
    void on_click_impl();


private:
    enum class State { CLOSED, OPENED };

    std::string m_header;
    std::vector<std::unique_ptr<UIButton>> m_items;
    bool m_header_hovered = false;
    State m_state = State::CLOSED;
};

class UIComponent
{
public:
    explicit UIComponent(const std::string& name)
        : m_name{name} {}

    void render();
    void set_rect(const Rectangle rect);

protected:
    virtual void render_impl() = 0;

protected:
    Rectangle m_outer_rect, m_inner_rect;

private:
    std::string m_name;
    static constexpr Color m_bg_color = { 242, 217, 191, 225 };
};