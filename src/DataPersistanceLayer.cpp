#include <iostream>
#include <format>
#include <fstream>

#include "Application.h"
#include "AppLayer.h"
#include "DataPersistanceLayer.h"

void DataPersitanceLayer::set_saves_root(std::filesystem::path root)
{
    m_root = root;
}

void DataPersitanceLayer::save_scene(const YAML::Node &scene)
{

    auto scene_data = Core::Application::get().get_layer<AppLayer>()->get_cur_scene();
    const auto dest_path = scene_data.full_path().replace_extension(".wsscene");
    std::cout << std::format("Saving {}", dest_path.string());
    std::ofstream fout(dest_path);
    fout << scene;
    std::cout << std::format("Saved in: {}\n", scene_data.full_path().string());
}

YAML::Node DataPersitanceLayer::load_scene(const std::filesystem::path& file)
{
    // TODO: das ist nur temporär
    std::cout << std::format("Try Load File: {}\n", file.string());
    return YAML::LoadFile(file.string());
}
