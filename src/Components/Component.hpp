#ifndef OGL_ENGINE_COMPONENT_HPP
#define OGL_ENGINE_COMPONENT_HPP

class Entity;

class Component
{
protected:
	explicit Component(const Entity& owner);
	const Entity* _owner;

public:
	virtual ~Component();

private:
	friend class Serializer;
};


#endif
