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


/* TODO: 
- wenn man canvas bewegt dann -> sieht man das die boundingbox vom focused element nicht mit
    transformiert wird
- screen move -> ctrl + left click
- scroll für zoom -> geschwindigkeit anpassen
- wenn man über scene label im ribbon hovered -> alle scenes für das projekt anzeigen
*/