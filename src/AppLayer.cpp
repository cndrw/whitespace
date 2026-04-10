#include <fstream>

#include "AppLayer.h"
#include "Application.h"
#include "CanvasLayer.h"
#include "DataPersistanceLayer.h"
#include "UILayer.h"


namespace Fields
{
    static constexpr const char* NAME { "name" };
    static constexpr const char* LAST_SCENE_OPEN { "lastSceneOpen" };
}

AppLayer::~AppLayer()
{
    std::filesystem::path path = (m_project_root / m_cur_proj_data.project_name).replace_extension(".wsproj");

    try 
    {
        YAML::Node project_file = YAML::LoadFile(path.string());
        // for the moment it is handled the hard way -> just deleting 
        // maybe in future this would be handled with a pop-up (Sure closing not saved scene?)
        if (!m_cur_proj_data.scene_list.empty())
        {
            project_file[Fields::LAST_SCENE_OPEN] = m_cur_proj_data.scene_list[m_cur_proj_data.active_scene_idx].scene_name;
        }
        std::ofstream fout(path);
        fout << project_file;
        std::cout << "Project settings saved!\n";
        
    }
    catch (const YAML::BadFile& e)
    {
        std::cerr << std::format("[Error] Failed to save project file. Changed name of project file or project name?\n");
    }
}

AppLayer::Result<std::string> AppLayer::open_project(std::filesystem::path project_dir)
{
    if (!std::filesystem::exists(project_dir)) return {false, "Path is not valid."};

    if (std::filesystem::is_regular_file(project_dir)) return { false, "Only directories allowed" };

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
    m_cur_proj_data.project_name = proj_file_content[Fields::NAME].as<std::string>();
    m_cur_proj_data.last_opened_scene.scene_name = proj_file_content[Fields::LAST_SCENE_OPEN].as<std::string>();
    m_cur_proj_data.active_scene_idx = 0;
    m_cur_proj_data.scene_list.clear();

    // search for all scene files in this project
    for (const auto& entry : std::filesystem::recursive_directory_iterator(project_dir))
    {
        const auto e = entry.path();
        
        if (e.extension() == ".wsscene" && !entry.is_directory())
        {
            YAML::Node scene_content = YAML::LoadFile(e.string());
            if (scene_content["project"].as<std::string>() != m_cur_proj_data.project_name)
                continue;

            m_cur_proj_data.scene_list.push_back({
                .scene_name = e.stem().string(),
                .save_dir = e.parent_path()
            });
            std::cout << std::format("Found scene: {}\n", m_cur_proj_data.scene_list.back().scene_name);
        }
    }

    auto& app = Core::Application::get();
    app.get_asset_manager()->set_root(m_project_root);
    app.get_layer<DataPersitanceLayer>()->set_saves_root((m_project_root).string());
    app.get_layer<UILayer>()->set_asset_root(m_project_root.string());
    app.get_layer<CanvasLayer>()->clear_scene();
    app.get_layer<UILayer>()->get_component<Hierachy>()->clear_all();

    if (!m_cur_proj_data.scene_list.empty())
    {
        auto it = std::ranges::find_if(m_cur_proj_data.scene_list,
            [&sn = m_cur_proj_data.last_opened_scene.scene_name] (const auto& s) { return sn == s.scene_name; });

        SceneData startin_scene = it == m_cur_proj_data.scene_list.end() ? m_cur_proj_data.scene_list[0] : *it;

        load_scene(startin_scene);
    }

    on_project_update.invoke(m_cur_proj_data);

    return {true, ""};
}

AppLayer::Result<std::string> AppLayer::create_project(std::filesystem::path project_path)
{
    // check if path is valid 
    // check if this file not already exists
    if (std::filesystem::exists(project_path)) return { false, "Project already exists." };

    // create yaml node by proj file template
    YAML::Node content;
    content[Fields::NAME] = project_path.stem().string();
    content[Fields::LAST_SCENE_OPEN] = "";

    // create and write .wsproj file
    std::ofstream outfile(project_path);
    outfile << content;
    outfile.close();

    // open new project 
    open_project(project_path.parent_path());

    return { true, "" };
}

void AppLayer::add_scene(SceneData scene)
{
    m_cur_proj_data.scene_list.push_back(scene);

    on_project_update.invoke(m_cur_proj_data);
}

void AppLayer::load_scene(const std::string& scene_name)
{
    // TODO: should be handle better... but for now only this is only here, using the base function
    // results in unwanted behaviour when loading the first scene after opening the project
    auto* c_layer = Core::Application::get().get_layer<CanvasLayer>();
    c_layer->save_scene();

    const auto scene =
        *std::ranges::find_if(m_cur_proj_data.scene_list, [scene_name] (const auto& scene) {
            return scene.scene_name == scene_name;
        });

    load_scene(scene);
}

void AppLayer::load_scene(const SceneData& scene)
{
    auto& app = Core::Application::get();
    app.get_layer<UILayer>()->get_component<Hierachy>()->clear_all(); // TODO: actually not the task of applayer...

    std::cout << std::format("path: {}\n name: {}\n", scene.save_dir.string(), scene.scene_name);

    auto data = app.get_layer<DataPersitanceLayer>()->load_scene(scene.full_path().replace_extension(".wsscene"));
    app.get_layer<CanvasLayer>()->load_scene(data);

    m_cur_proj_data.active_scene_idx =
        std::ranges::find(m_cur_proj_data.scene_list, scene)
        - m_cur_proj_data.scene_list.begin();

    m_cur_proj_data.last_opened_scene = scene;

    on_project_update.invoke(m_cur_proj_data);
    std::cout << std::format("Switched to scene: {}\n", scene.scene_name);
}

std::optional<SceneData> AppLayer::get_cur_scene() const
{
    if (m_cur_proj_data.scene_list.empty())
    {
        std::cout << "This project has no scenes yet.\n";
        return std::nullopt;
    }

    return m_cur_proj_data.scene_list[m_cur_proj_data.active_scene_idx];
}

