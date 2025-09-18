#pragma once

namespace patterns
{
    template <typename T>
    class Singleton
    {
    protected:
        Singleton() = default;

    public:
        Singleton(const Singleton &) = delete;
        Singleton(Singleton &&) = delete;
        Singleton &operator=(const Singleton &) = delete;
        Singleton &operator=(Singleton &&) = delete;

        static T &GetInstance();
    };

    template <typename T>
    T &Singleton<T>::GetInstance()
    {
        static T instance;
        return instance;
    }
}
