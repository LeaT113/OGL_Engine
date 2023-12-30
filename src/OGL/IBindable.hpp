#ifndef OGL_ENGINE_IBINDABLE_HPP
#define OGL_ENGINE_IBINDABLE_HPP

// TODO Add virtual destructor?
class IBindable
{
public:
    virtual unsigned int GetBindID() const = 0;
};

#endif
