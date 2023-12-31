#ifndef OGL_ENGINE_IBINDABLE_HPP
#define OGL_ENGINE_IBINDABLE_HPP


class IBindable
{
public:
    virtual ~IBindable() = default;

    virtual unsigned int GetBindID() const = 0;
};

#endif
