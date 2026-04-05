#include <iostream> 
#include <format>

#include "Application.h"
#include "AppLayer.h"
#include "AssetManager.h"

#include "raylib.h"

namespace Core
{
    void AssetManager::add_asset(const std::filesystem::path& path)
    {
        m_assets[path.stem().string()] = (Asset) {
            .rel_path = std::filesystem::relative(path, m_root),
            .name = path.stem().string(),
            .texture = LoadTexture(path.string().c_str()),
            .ppu = 32 // TODO: set default back to 100
        };
    }

    Asset AssetManager::add_asset_from_relative(std::filesystem::path path)
    {
        return m_assets[path.stem().string()] = (Asset) {
            .rel_path = path,
            .name = path.stem().string(),
            .texture = LoadTexture((m_root / path).string().c_str()),
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