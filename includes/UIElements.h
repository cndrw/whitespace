#pragma once

#include <functional>

#include "raylib.h"


class UIElement
{
using Callback = std::function<void()>;
public:
    UIElement() = default;

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
using Callback = std::function<void()>;
public:
    UIButton() = default;

    UIButton(const Rectangle& rect, Callback render, Callback on_click)
        : UIButton(rect, render, on_click, [](){}) {}

    UIButton(const Rectangle& rect, Callback render, Callback on_click, Callback on_hover)
        : UIElement(rect, render), on_click(on_click), on_hover(on_hover) {}

    virtual ~UIButton() = default;

    virtual bool is_hovered() override;

public:
    Callback on_click;
    Callback on_hover;
};

class UIDropDownList : public UIButton 
{
using Callback = std::function<void()>;
public:
    UIDropDownList() = default;

    UIDropDownList(const Rectangle& rect, const std::string& header)
        : UIDropDownList(rect, header, {}) {}

    UIDropDownList(
        const Rectangle& rect_p,
        const std::string& header,
        const std::vector<std::string>& items);

    virtual ~UIDropDownList() = default;

    inline void clear_items() { m_items.clear(); }
    void add_item(const std::string& item, Callback on_click);
    inline void set_header(const std::string& header) { m_header = header; }
    virtual bool is_hovered() override;


private:
    enum class State { CLOSED, OPENED };

    std::string m_header;
    std::vector<std::unique_ptr<UIButton>> m_items;
    bool m_header_hovered = false;
    State m_state = State::CLOSED;
};