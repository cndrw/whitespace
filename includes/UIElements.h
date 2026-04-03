#pragma once

#include <functional>
#include <memory>

#include "raylib.h"

#include "SceneElement.h"

class UIElement
{
public:
    using Callback = std::function<void()>;
    UIElement() = default;

    UIElement(const Rectangle& rect)
        : rect(rect), render([](){}) {}

    UIElement(const Rectangle& rect, Callback render)
        : rect(rect), render(render) {}

    inline void set_pos(const Vec2& pos) { rect.x = pos.x; rect.y = pos.y; }

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
    Color fill_color = RAYWHITE;
    bool use_fill = false;
};

class UITextBox : public UIElement
{
public:
    UITextBox();
    virtual ~UITextBox() override = default;

    bool process_input();
    void on_click();
    void cancel_edit();
    virtual bool is_hovered() override;

    void set_text(const std::string& text);
    std::string get_text() const { return std::string(m_buffer); };
    void set_static(bool status);

private:
    virtual void render_impl();

public:
    Callback on_edited;

private:
    static constexpr auto MAX_BUFFER_SIZE { 20 };
    char m_buffer[MAX_BUFFER_SIZE];
    bool m_edit_mode = false;
    bool m_draw_label = false;
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

class UIScrollView : public UIElement
{

struct Entry { bool active; std::unique_ptr<UIButton> button; };

public:
    UIScrollView();
    bool process_input();
    void add_entry(const SpriteElement& elem, Callback on_click);
    void remove_entry(const SpriteElement& elem);
    void update_entry(const std::string_view before, const std::string_view after);
    void handle_scrolling();
    bool check_if_in_view(const Entry& entry) const;
    void clear();

private:
    Rectangle get_new_rect() const;
    void render_impl() const;

private:
    // when UIButton is rid of this horrendous internal [this] capture 
    // the ptr should be be able to be droppped...
    // also UIButton maybe manages hiw own state "active"?
    std::vector<Entry> m_entries;
};

class UIWindowBase : public UIElement
{
public:
    UIWindowBase(const Rectangle& rect, const char* name);
    void open();
    void close();
    // bool process_input();
    virtual void render_content() = 0;

    bool is_open() const;
private:
    void render_impl();

protected:
    Rectangle m_content_rect;

private:
    // TODO: see above
    std::vector<std::unique_ptr<UIButton>> m_buttons;
    const char* m_title;
    enum class State { CLOSED, OPEN };
    State m_state = State::CLOSED;
};

// TODO: should probably be somewhere else
class OpenProjectWindow : public UIWindowBase
{
public:
    OpenProjectWindow(const Rectangle& rect);
    bool process_input();
    void render_content() override;

private:
    // TODO: ptr...
    std::unique_ptr<UITextBox> m_textbox;
    std::unique_ptr<UIButton> m_open_button;
};

class UIComponent
{
public:
    explicit UIComponent(const std::string& name)
        : m_name{name} {}

    virtual void render();
    void set_rect(const Rectangle rect);

protected:
    virtual void render_impl() {};

protected:
    Rectangle m_outer_rect, m_inner_rect;
    static constexpr Color m_bg_color = { 242, 217, 191, 225 };

private:
    std::string m_name;
};