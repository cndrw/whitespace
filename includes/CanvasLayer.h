#pragma once

#include <map>
#include <vector>
#include <optional>
#include <filesystem>

#include "raylib.h"

#include "Layer.h"
#include "SceneElement.h"
#include "Event.h"
#include "AssetExplorer.h"
#include "AssetManager.h"


class CanvasLayer : public Core::Layer
{
public:
    CanvasLayer();
    virtual void init() override;
    virtual void update() override;
    virtual void render() override;
    virtual bool process_input() override;
    virtual ~CanvasLayer();
    void load_scene(const std::string& scene_name);
    Core::Event<const std::optional<SpriteElement>&> on_element_changed;

private:
    std::shared_ptr<SpriteElement> determine_focused_element(const Vector2 cursor_pos);
    void draw_arrow(int width, int height, float angle, Color color) const;
    void draw_reference_resolution(Vector2 res);
    void add_scene_element(const Core::Asset& asset);
    std::string resolve_naming(const std::filesystem::path& path) const;
    void save_scene();

    // tmp functions
    Rectangle transform_to_screen(const Rectangle& rect);

private:
    std::map<uint8_t, std::vector<std::shared_ptr<SpriteElement>>> m_sprite_elements;
    std::shared_ptr<SpriteElement> m_focused_sprite_elem;
    Vector2 m_sprite_drag_offset;
    Vector2 m_origin;
    float m_scale = 1;
};
