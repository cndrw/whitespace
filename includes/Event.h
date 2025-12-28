#pragma once

#include <functional>
#include <vector>
#include <ranges>
#include <unordered_map>

namespace Core
{
    template<typename... TArgs>
    class Event
    {
    public:
        using CallbackFn = std::function<void(TArgs...)>;
        int add_listener(CallbackFn func)
        {
            m_listener[++current_id] = func;
            return current_id;
        }

        void remove_listener(int id)
        {
            m_listener.erase(id);
        } 

        void invoke(TArgs... args)
        {
            if (m_listener.size() > 0)
            {
                for (const auto& [_, fn] : m_listener)
                {
                    fn(args...);
                }
            }
            else
            {
                std::cout << "No listener registrated!\n";
            }
        }

    private:
        std::unordered_map<int, CallbackFn> m_listener;
        int current_id = 0;
    };
}
