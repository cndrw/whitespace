#include "AppLayer.h"
#include "Application.h"
#include "CanvasLayer.h"
#include "DataPersistanceLayer.h"
#include "UILayer.h"

void AppLayer::open_project()
{
    m_project_root = std::filesystem::path(R"(D:\stuff\repos\whitespace\example)");
    auto& app = Core::Application::get();
    app.get_layer<DataPersitanceLayer>()->set_saves_root((m_project_root / "scenes").string());
    app.get_layer<UILayer>()->set_asset_root((m_project_root / "assets").string());
    app.get_layer<CanvasLayer>()->load_scene("scene.yaml");
}