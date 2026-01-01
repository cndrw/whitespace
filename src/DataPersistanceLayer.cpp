#include <iostream>
#include <format>
#include <fstream>

#include "DataPersistanceLayer.h"

void DataPersitanceLayer::set_saves_root(std::filesystem::path root)
{
    m_root = root;
}

void DataPersitanceLayer::save_scene(const YAML::Node &scene)
{
    const auto dest_path = m_root / "scene.yaml";
    std::ofstream fout(dest_path);
    fout << scene;
    std::cout << std::format("Saved in: {}\n", dest_path.string());
}

YAML::Node DataPersitanceLayer::load_scene(const std::filesystem::path& file)
{
    std::cout << std::format("Try Load File: {}\n", (m_root / file).string());
    return YAML::LoadFile((m_root / file).string());
}
