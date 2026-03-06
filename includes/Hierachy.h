#pragma once

#include <cstdint>
#include <string>

#include "UIElements.h"
#include "SceneElement.h"
#include "Event.h"

class Hierachy : public UIComponent
{
public:
    Hierachy();
    ~Hierachy();

    bool process_input();
    void add_entry(const SpriteElement& elem);
    void remove_entry(const SpriteElement& elem);

    Core::Event<const uint16_t> on_element_selected;

protected:
    void render_impl() override;

private:
    Rectangle get_entry_rect(uint16_t pos) const;
    void arrange_entries();

private:
    std::vector<UIButton*> m_entries;
};
