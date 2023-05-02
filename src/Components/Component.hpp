#ifndef OGL_ENGINE_COMPONENT_HPP
#define OGL_ENGINE_COMPONENT_HPP

class Entity;

class Component
{
protected:
	Component();
	const Entity* _owner;

public:
	virtual ~Component();
};


#endif
