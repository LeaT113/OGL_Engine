#ifndef OGL_ENGINE_ENTITY_HPP
#define OGL_ENGINE_ENTITY_HPP
#include <memory>
#include <map>
#include "Components/Component.hpp"
#include "Components/TransformComponent.hpp"


class Entity
{
public:
	Entity();

	size_t ID() const;


	// Components
	template <typename T, typename... Args>
	Entity& AddComponent(Args&&... args);

	template <typename T>
	T* GetComponent() const;


	// Shortcuts
	TransformComponent* Transform();

private:
	// Static
	static size_t _sIndexCounter;

	size_t _id;

	// Components
	std::map<const std::type_info*, std::unique_ptr<Component>> _components;
};


template<typename T, typename... Args>
Entity& Entity::AddComponent(Args &&... args)
{
	auto [it, isNew] = _components.try_emplace(&typeid(T), std::make_unique<T>(this, std::forward<Args>(args)...));

	return *this;
}

/// Get component of type
/// \tparam T Type of component, must inherit from Component
/// \return Pointer to component or nullptr if Entity has no component of this type
template<typename T>
T* Entity::GetComponent() const
{
	auto it = _components.find(&typeid(T));
	if(it == _components.end())
		return nullptr;

	return dynamic_cast<T*>(it->second.get());
}

#endif //OGL_ENGINE_ENTITY_HPP
