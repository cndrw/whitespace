#include "application.h"
#include "layer.h"

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "style_candy.h"

static Application* s_application = nullptr;

Application::Application()
{
    s_application = this;
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);  // Window configuration flags
    InitWindow(800, 400, "editor");
    SetTargetFPS(60);
    GuiLoadStyleCandy();
}

void Application::run()
{
    m_is_running = true;
    while (m_is_running)
    {

        if (WindowShouldClose())
        {
            stop();
            break;
        }

        for (const auto& layer : m_layers)
        {
            layer->update();
        }

        BeginDrawing();
        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
        for (const auto& layer : m_layers)
        {
            layer->render();
        }
        EndDrawing();
    }
    CloseWindow();
}

void Application::stop()
{
    m_is_running = false;
}


Application& Application::get()
{
    return *s_application;
}
