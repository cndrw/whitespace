#include "Application.h"
#include "CanvasLayer.h"
#include "UILayer.h"
#include "AppLayer.h"
#include "DataPersistanceLayer.h"

#include <filesystem>

#include "yaml-cpp/yaml.h"


int main()
{
    Core::Application app{};
    app.push_layer<DataPersitanceLayer>();
    app.push_layer<AppLayer>();
    app.push_layer<CanvasLayer>();
    app.push_layer<UILayer>();
    app.run();
    return 0;
}


