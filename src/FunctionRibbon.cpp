#include <memory>
#include <iostream>

#include "raygui.h"

#include "FunctionRibbon.h"
#include "Application.h"
#include "AppLayer.h"

void FunctionRibbon::render()
{
    
   if (GuiLabelButton({ 0, 0, 50, 15 }, GuiIconText(GuiIconName::ICON_FILE_OPEN, "Load")))
   {
        Core::Application::get().get_layer<AppLayer>()->open_project(); 
   }
}

void FunctionRibbon::set_rect(Rectangle rect)
{
    m_rect = std::move(rect);
}
