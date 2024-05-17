#ifndef OGL_ENGINE_SINGLETON_HPP
#define OGL_ENGINE_SINGLETON_HPP

template <typename T>
class Singleton
{
public:
	static T& Instance()
	{
		static T instance;
		return instance;
	}

protected:
	Singleton() = default;
	~Singleton() = default;

public:
	Singleton(Singleton &other) = delete;
	Singleton(Singleton const &) = delete;
	Singleton& operator=(Singleton const &) = delete;
};


#endif
