#pragma once

#include <filesystem>

#include "Layer.h"
#include "Event.h"
#include "Inspector.h"
#include "FunctionRibbon.h"
#include "AssetExplorer.h"

class UILayer : public Core::Layer
{
public:
    UILayer();
    virtual void init() override;
    bool process_input() override;
    virtual void update() override;
    virtual void render() override;
    virtual ~UILayer();
    void set_asset_root(std::filesystem::path path);
    
    template<typename T>
    requires(std::derived_from<T, UIComponent>)
    constexpr T* get_component()
    {
        if constexpr (std::is_same_v<T, AssetExplorer>) return &m_asset_explorer;
        if constexpr (std::is_same_v<T, Inspector>) return &m_inspector;
        if constexpr (std::is_same_v<T, FunctionRibbon>) return &m_func_ribbon;
        return nullptr;
    }

private:
    std::vector<Rectangle> m_clickable_obj;
    Inspector m_inspector;
    AssetExplorer m_asset_explorer;
    FunctionRibbon m_func_ribbon;
};