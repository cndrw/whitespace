#include "Application.h"
#include "CanvasLayer.h"
#include "UILayer.h"

int main()
{
    Core::Application app{};
    app.push_layer<CanvasLayer>();
    app.push_layer<UILayer>();
    app.run();
    return 0;
}


