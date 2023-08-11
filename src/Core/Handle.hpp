#ifndef OGL_ENGINE_HANDLE_HPP
#define OGL_ENGINE_HANDLE_HPP

#include <memory>
#include <utility>

template<typename T>
class Handle
{
    // TODO Add error when accessing handle that's been moved
public:
    Handle() = delete;

    /*
    template<typename ...Args>
    explicit Handle(Args&&... args) : _ptr(std::make_unique<T>(std::forward<Args>(args)...)) {}*/

    // Move constructor
    Handle(Handle&& other) noexcept : _ptr(std::move(other._ptr))
    { }

    // Move assignment operator
    Handle& operator=(Handle&& other) noexcept
    {
        _ptr = std::move(other._ptr);
        return *this;
    }

    template<typename... Args>
    static Handle Make(Args&&... args)
    {
        return Handle(std::make_unique<T>(std::forward<Args>(args)...));
    }

    Handle Release()
    {
        return Handle(std::move(_ptr));
    }

    T* Access() const
    {
        return _ptr.get();
    }

    // Access the object
    T* operator->() const { return _ptr.get(); }
    T& operator*() const { return *_ptr; }

private:
    std::unique_ptr<T> _ptr;

    explicit Handle(std::unique_ptr<T> ptr) : _ptr(std::move(ptr))
    { }

    static Handle Take(Handle other)
    {
        return Handle(std::move(other._ptr));
    }
};


#endif