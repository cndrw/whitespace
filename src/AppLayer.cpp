#include "AppLayer.h"
#include "Application.h"
#include "CanvasLayer.h"
#include "DataPersistanceLayer.h"
#include "UILayer.h"

AppLayer::Result<std::string> AppLayer::open_project(std::filesystem::path project_dir)
{
    if (!std::filesystem::exists(project_dir)) return {false, "Path is not valid."};

    std::filesystem::path proj_file;

    // search for project file (there can only be one in a directory)
    for (const auto& entry : std::filesystem::directory_iterator(project_dir))
    {
        if (entry.path().extension() == ".wsproj")
        {
            proj_file = entry.path();
        }
    }

    if (proj_file.empty()) return {false, "No project file found."};

    m_project_root = project_dir;
    YAML::Node proj_file_content = YAML::LoadFile(proj_file.string());
    m_cur_proj_data.project_name = proj_file_content["name"].as<std::string>();
    m_cur_proj_data.last_opened_scene.scene_name = proj_file_content["lastSceneOpen"].as<std::string>();

    // search for all scene files in this project
    for (const auto& entry : std::filesystem::recursive_directory_iterator(project_dir))
    {
        const auto e = entry.path();
        
        if (e.extension() == ".wsscene" && !entry.is_directory())
        {
            m_cur_proj_data.scene_list.push_back({
                .scene_name = e.stem().string(),
                .save_dir = e.parent_path()
            });
            std::cout << std::format("Found scene: {}\n", m_cur_proj_data.scene_list.back().scene_name);

            if (e.stem().string() == m_cur_proj_data.last_opened_scene.scene_name)
            {
                m_cur_proj_data.last_opened_scene.save_dir = e.parent_path();
            }
        }
    }

    auto& app = Core::Application::get();
    app.get_asset_manager()->set_root(m_project_root);
    app.get_layer<DataPersitanceLayer>()->set_saves_root((m_project_root / "scenes").string());
    app.get_layer<UILayer>()->set_asset_root(m_project_root.string());

    auto data = app.get_layer<DataPersitanceLayer>()
        ->load_scene(m_cur_proj_data.last_opened_scene.full_path().replace_extension(".wsscene"));
    app.get_layer<CanvasLayer>()->load_scene(data);


    m_cur_proj_data.project_name = proj_file.stem().string();

    
    m_cur_proj_data.active_scene_idx =
        std::ranges::find(m_cur_proj_data.scene_list, m_cur_proj_data.last_opened_scene)
        - m_cur_proj_data.scene_list.begin();

    std::cout << std::format("Opened scene: {} ({})\n", m_cur_proj_data.last_opened_scene.scene_name, m_cur_proj_data.active_scene_idx);
    on_project_update.invoke(m_cur_proj_data);

    return {true, ""};
}

void AppLayer::load_scene(const std::string& scene_name)
{
    auto& app = Core::Application::get();
    app.get_layer<UILayer>()->get_component<Hierachy>()->clear_all();
    auto* c_layer = app.get_layer<CanvasLayer>();
    c_layer->save_scene();

    const auto scene =
        std::ranges::find_if(m_cur_proj_data.scene_list, [scene_name] (const auto& scene) {
            return scene.scene_name == scene_name;
        })->full_path().replace_extension(".wsscene");

    auto data = app.get_layer<DataPersitanceLayer>()->load_scene(scene);
    c_layer->load_scene(data);

    for (size_t i = 0; i < m_cur_proj_data.scene_list.size(); i++)
    {
        if (m_cur_proj_data.scene_list[i].scene_name == scene_name)
        {
            m_cur_proj_data.active_scene_idx = i;
            break;
        }
    }

    on_project_update.invoke(m_cur_proj_data);
    std::cout << std::format("Switched to scene: {}\n", scene_name);
}

SceneData AppLayer::get_cur_scene() const
{
    return m_cur_proj_data.scene_list[m_cur_proj_data.active_scene_idx];
}

