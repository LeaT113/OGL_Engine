#ifndef OGL_ENGINE_COMPONENT_HPP
#define OGL_ENGINE_COMPONENT_HPP

class TransformComponent;
class Entity;

class Component
{
public:
	virtual ~Component();

	TransformComponent* GetTransform() const;

	const Entity& GetEntity() const;

protected:
	explicit Component(const Entity& owner);
	const Entity* _owner;

private:
	friend class Serializer;
};


#endif
