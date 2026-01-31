#include <iostream>

#include <ranges>

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "style_candy.h"

#include "application.h"
#include "layer.h"
#include "Utils.h"


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

            // process input 
            // layers receive permission to process user input one by one
            // if one layer uses the input, it can mark this frame as handled
            for (const auto& layer : m_layers | std::views::reverse)
            {
                if (layer->process_input())
                {
                    break;
                }
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

    AssetManager *Application::get_asset_manager()
    {
        return &m_asset_manager;
    }

    Application& Application::get()
    {
        return *s_application;
    }

    float Application::get_time()
    {
        return GetTime();
    }

}
