#include <iostream>

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "style_candy.h"

#include "application.h"
#include "layer.h"


namespace Core
{
    constexpr auto WINDOW_WIDTH { 1280 };
    constexpr auto WINDOW_HEIGHT { 720 };

    static Application* s_application = nullptr;

    Application::Application()
    {
        s_application = this;
        SetConfigFlags(FLAG_WINDOW_RESIZABLE);  // Window configuration flags
        InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Whitespace");
        SetTargetFPS(60);
        GuiLoadStyleCandy();
        GuiSetStyle(DEFAULT, TEXT_SIZE, 15);
    }

    void Application::run()
    {
        for (const auto& layer : m_layers)
        {
            layer->init();
        }

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

}
