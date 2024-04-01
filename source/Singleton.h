#pragma once

#include <mutex>

template <typename T>
class Singleton
{
protected:
    // Protected constructor to prevent direct instantiation
    Singleton() = default;

public:
    // Deleted copy constructor and assignment operator for singleton behavior
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    // Public static method to access the singleton instance
    static T& getInstance()
    {
        static std::once_flag onceFlag;
        static T* instance = nullptr;
        std::call_once(onceFlag, []()
            {
                instance = new T();
            });
        return *instance;
    }
};