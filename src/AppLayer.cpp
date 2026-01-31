#include "AppLayer.h"
#include "Application.h"
#include "CanvasLayer.h"
#include "DataPersistanceLayer.h"
#include "UILayer.h"

void AppLayer::open_project()
{
    m_project_root = std::filesystem::path(R"(D:\Mein stuff\Ordner\Privat\Projects\whitespace\example)");
    const auto proj_file = m_project_root / "project.yaml";
    const auto scene_name = "house.yaml";

    auto& app = Core::Application::get();
    app.get_layer<DataPersitanceLayer>()->set_saves_root((m_project_root / "scenes").string());
    app.get_layer<UILayer>()->set_asset_root((m_project_root / "assets").string());
    app.get_layer<CanvasLayer>()->load_scene(scene_name);

    m_cur_proj_data.project_name = proj_file.stem().string();

    for (const auto& entry : std::filesystem::directory_iterator(m_project_root / "scenes"))
    {
        const auto path = entry.path();
        if (path.extension() == ".yaml")
        {
            m_cur_proj_data.scene_list.push_back(path.stem().string());
        }
    }

    m_cur_proj_data.active_scene_idx = 0;
    on_project_update.invoke(m_cur_proj_data);
}

void AppLayer::load_scene(const std::string& scene_name)
{
    auto* c_layer = Core::Application::get().get_layer<CanvasLayer>();
    c_layer->save_scene();
    c_layer->load_scene(std::string{scene_name} += ".yaml");

    for (size_t i = 0; i < m_cur_proj_data.scene_list.size(); i++)
    {
        if (m_cur_proj_data.scene_list[i] == scene_name)
        {
            m_cur_proj_data.active_scene_idx = i;
            break;
        }
    }
    for (auto a : m_cur_proj_data.scene_list)
    {
        std::cout << a << std::endl;
    }
    on_project_update.invoke(m_cur_proj_data);
    std::cout << "Switched to scene: " << scene_name << std::endl;
}

std::string AppLayer::get_cur_scene_name() const
{
    return m_cur_proj_data.scene_list[m_cur_proj_data.active_scene_idx];
}

