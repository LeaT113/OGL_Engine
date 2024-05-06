#ifndef OGL_ENGINE_COMPONENT_HPP
#define OGL_ENGINE_COMPONENT_HPP

class TransformComponent;
class Entity;

class Component
{
protected:
	explicit Component(const Entity& owner);
	const Entity* _owner;

public:
	virtual ~Component();

	TransformComponent* GetTransform() const;

	const Entity& GetEntity() const;

private:
	friend class Serializer;
};


#endif
