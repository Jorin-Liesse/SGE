#pragma once
#include <functional>
#include <vector>
#include <algorithm>

namespace patterns
{
    template <typename... Args>
    class Event
    {
    public:
        using Callback = std::function<void(Args...)>;
        using ID = int;

        ID subscribe(const Callback& callback)
        {
            ID id = nextId++;
            listeners.push_back({ id, callback });
            return id;
        }

        void unsubscribe(ID id)
        {
            listeners.erase(
                std::remove_if(listeners.begin(), listeners.end(),
                               [id](const auto& pair) { return pair.first == id; }),
                listeners.end());
        }

        void invoke(Args... args)
        {
            for (auto& [id, listener] : listeners)
                listener(args...);
        }

    private:
        std::vector<std::pair<ID, Callback>> listeners;
        ID nextId = 1;
    };
}
