#ifndef OGL_ENGINE_HANDLE_HPP
#define OGL_ENGINE_HANDLE_HPP

#include <memory>
#include <utility>

template<typename T>
class Handle
{
    // TODO Add error when accessing handle that's been moved
public:
    Handle() = default;

    // Move constructor
    Handle(Handle&& other) noexcept : _ptr(std::move(other._ptr))
    { }

    // Move assignment operator
    Handle& operator=(Handle&& other) noexcept
    {
        _ptr = std::move(other._ptr);
        return *this;
    }

    // Polymorphic move constructor
    template<typename U, typename = std::enable_if_t<std::is_base_of_v<T, U>>>
    explicit Handle(Handle<U>&& other) noexcept : _ptr(other.release())
    { }

    // Creation
    template<typename... Args>
    static Handle Make(Args&&... args)
    {
        return Handle(std::make_unique<T>(std::forward<Args>(args)...));
    }
    static Handle Empty()
    {
        return Handle(nullptr);
    }

    Handle Pass()
    {
        return Handle(std::move(_ptr));
    }
    T Release()
    {
        return std::move(*_ptr);
    }

    T* Access() const
    {
        return _ptr.get();
    }

    // Access the object
    T* operator->() const { return _ptr.get(); }
    T& operator*() const { return *_ptr; }

    std::unique_ptr<T> release() noexcept {
        return std::move(_ptr);
    }

private:
    std::unique_ptr<T> _ptr;

    // Private constructor used by Make
    explicit Handle(std::unique_ptr<T> ptr) : _ptr(std::move(ptr))
    { }
};


#endif