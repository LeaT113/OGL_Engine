#ifndef OGL_ENGINE_ENTITY_HPP
#define OGL_ENGINE_ENTITY_HPP
#include <memory>
#include <map>
#include <typeindex>

#include "../Components/Component.hpp"
#include "../Core/Handle.hpp"

class TransformComponent;

class Entity
{
public:
	explicit Entity(std::string name = "");

	size_t ID() const;
	const std::string& GetName() const;


	// Components
	template <typename T, typename... Args>
	Entity& AddComponent(Args&&... args);

	template <typename T>
	T* GetComponent() const;

	// Shortcuts
	TransformComponent* GetTransform() const;

private:
	friend class Serializer;

	// Static
	static size_t _sIndexCounter;

	size_t _id;
	std::string _name;

	// Components
	std::map<std::type_index, Handle<Component>> _components;
	mutable TransformComponent* _transform = nullptr;
};


template<typename T, typename... Args>
Entity& Entity::AddComponent(Args &&... args)
{
	auto [it, isNew] = _components.try_emplace(typeid(T), Handle<T>::Make(*this, std::forward<Args>(args)...));

	return *this;
}

/// Get component of type
/// \tparam T Type of component, must inherit from Component
/// \return Pointer to component or nullptr if Entity has no component of this type
template<typename T>
T* Entity::GetComponent() const
{
	auto it = _components.find(typeid(T));
	if(it == _components.end())
		return nullptr;

	return dynamic_cast<T*>(it->second.Access());
}

#endif //OGL_ENGINE_ENTITY_HPP
