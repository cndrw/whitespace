#include <iostream> 
#include <format>

#include "AssetManager.h"

#include "raylib.h"

namespace Core
{
    void AssetManager::add_asset(const std::filesystem::path& path)
    {
        m_assets[path.stem().string()] = (Asset) {
            .path = path,
            .name = path.stem().string(),
            .texture = LoadTexture(path.string().c_str()),
            .ppu = 32 // TODO: set default back to 100
        };
    }

    Asset AssetManager::get_asset(const Handle& handle) const
    {
        return m_assets.at(handle);
    }

    void AssetManager::update_asset(const Asset& asset)
    {
        auto& handle = asset.name;
        if (!exists(handle))
        {
            std::cerr << std::format("[AssetManager] Cannot update {} since it is not registered.\n", asset.name);
            return;
        }

        m_assets[handle] = asset;
        on_asset_update.invoke(handle);
    }

    bool AssetManager::exists(const Handle& handle) const
    {
        return m_assets.contains(handle);
    }
}