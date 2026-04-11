#pragma once

#include <filesystem>
#include <vector>
#include <string>

#include "yaml-cpp/yaml.h"

#include "Layer.h"
#include "Event.h"

typedef struct SceneData {
    std::string scene_name;
    std::filesystem::path save_dir;

    inline std::filesystem::path full_path() const { return std::filesystem::path(save_dir).append(scene_name); }

    bool operator==(const SceneData& second) const
    {
        return full_path() == second.full_path();
    }
} SceneData;

typedef struct {
    uint16_t active_scene_idx;
    std::string project_name;
    SceneData last_opened_scene;
    std::vector<SceneData> scene_list;
} ProjectMetadata;

class AppLayer : public Core::Layer
{
public:
    template<typename T>
    using Result = std::pair<bool, T>;

    AppLayer() = default;
    void init() final override;
    // virtual void update() override;
    ~AppLayer();

    Result<std::string> open_project(std::filesystem::path project_file);
    Result<std::string> create_project(std::filesystem::path project_path);
    void load_scene(const std::string& scene_name);
    void load_scene(const SceneData& scene);
    std::optional<SceneData> get_cur_scene() const;
    void add_scene(SceneData scene);
    inline std::string get_cur_project_name() const { return m_cur_proj_data.project_name; }
    // TODO: i dont think this shoudl be handles like this
    inline std::vector<std::filesystem::path> get_recent_projects() const { return m_recent_projects; }

    Core::Event<const ProjectMetadata&> on_project_update; 

private:
    std::filesystem::path m_project_root;
    ProjectMetadata m_cur_proj_data;
    std::vector<std::filesystem::path> m_recent_projects;
};