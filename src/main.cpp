#include "Application.h"
#include "CanvasLayer.h"
#include "UILayer.h"
#include "DataPersistanceLayer.h"

#include <filesystem>

#include "yaml-cpp/yaml.h"


int main()
{
    Core::Application app{};
    std::filesystem::path path(R"(D:\stuff\repos\whitespace\example\scenes)");
    app.push_layer<DataPersitanceLayer>(path);
    app.push_layer<CanvasLayer>();
    app.push_layer<UILayer>();
    app.run();
    return 0;
}


