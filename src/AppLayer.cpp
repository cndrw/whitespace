#include "AppLayer.h"
#include "Application.h"
#include "CanvasLayer.h"
#include "DataPersistanceLayer.h"
#include "UILayer.h"

void AppLayer::open_project()
{
    m_project_root = std::filesystem::path(R"(D:\Mein stuff\Ordner\Privat\Projects\whitespace\example)");
    const auto proj_file = m_project_root / "project.yaml";
    const auto scene_name = "scene.yaml";

    auto& app = Core::Application::get();
    app.get_layer<DataPersitanceLayer>()->set_saves_root((m_project_root / "scenes").string());
    app.get_layer<UILayer>()->set_asset_root((m_project_root / "assets").string());
    app.get_layer<CanvasLayer>()->load_scene(scene_name);

    m_cur_proj_data.project_name = proj_file.stem().string();
    m_cur_proj_data.active_scene_name = scene_name;
    on_project_update.invoke(m_cur_proj_data);
}