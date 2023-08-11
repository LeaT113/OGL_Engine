#ifndef OGL_ENGINE_HYBRIDSINGLETON_HPP
#define OGL_ENGINE_HYBRIDSINGLETON_HPP


template <typename T>
class HybridSingleton
{
public:
	HybridSingleton()
	{
		if (_sInstance != nullptr)
			throw std::runtime_error("HybridSingleton attempted to create second instance");
		_sInstance = (T*)this;
	}

	~HybridSingleton()
	{
		_sInstance = nullptr;
	}

	HybridSingleton(HybridSingleton &other) = delete;

	HybridSingleton(HybridSingleton const &) = delete;

	HybridSingleton &operator=(HybridSingleton const &) = delete;

protected:
	static T& Instance()
	{
		if (_sInstance == nullptr)
			throw std::runtime_error("HybridSingleton accessed instance before constructing");
		return *_sInstance;
	}

private:
	static T *_sInstance;
};

template<typename T>
T* HybridSingleton<T>::_sInstance = nullptr;

#endif