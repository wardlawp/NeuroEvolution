#include "Entity.h"
#include "EntityComponent.h"


Entity::Entity()
	: m_awake(true)
	, m_components{nullptr}
{
}


Entity::~Entity()
{
	for (auto& ptr : m_components)
	{
		if (ptr != nullptr)
		{
			delete ptr;
		}
	}
}

bool Entity::IsAwake() const
{
	return m_awake;
}

void Entity::Awake()
{
	m_awake = true;
}

void Entity::Sleep()
{
	m_awake = false;
}

Entity::Status Update()
{
	return Entity::Status::Sleeping;
}


template<typename T>
T* Entity::GetComponent()
{
	for (auto& ptr : m_components)
	{
		if (ptr == nullptr)
		{
			break;
		}

		if (dynamic_cast<T*>(ptr) != nullptr)
		{
			return static_cast<T*>(ptr);
		}
	}

	return nullptr;

}

template<typename T>
const T* Entity::GetComponent() const
{
	Entity& nonConstThis  = const_cast<Entity&>(*this);
	const T* result = nonConstThis.GetComponent<T>();
	return result;
}


// Get Component Instantiations
template const BodyComponent* Entity::GetComponent() const;
template BodyComponent* Entity::GetComponent();
template const HealthComponent* Entity::GetComponent() const;
template HealthComponent* Entity::GetComponent();

void Entity::AddComponent(EntityComponent* component)
{
	for (auto& slot : m_components)
	{
		if (slot == nullptr)
		{
			slot = component;
			return;
		}

		if (typeid(slot) == typeid(component))
		{
			THROW("Entity should not have 2 of the same component");
		}

	}

	THROW("Entity could not add component");
}

Wall::Wall(const Vec2f p1, const Vec2f p2, float thickness)
{
	Sleep();

	if (!(p1.x == p2.x || p1.y == p2.y))
	{
		THROW("Wall inputs must be orthogonal");
		return;
	}

	const float centerX = (p1.x + p2.x) / 2.0f;
	const float centerY = (p1.y + p2.y) / 2.0f;
	const float width = std::abs(p1.x - p2.x) + thickness;
	const float height = std::abs(p1.y - p2.y) + thickness;

	BodyComponent* component = new BodyComponent(centerX, centerY, width, height, Colour::Grey);
	AddComponent(component);
}

Target::Target(const Vec2f p1, float thickness)
{
	Sleep();
	BodyComponent* component = new BodyComponent(p1.x, p1.y, thickness, thickness, Colour::Green, false);
	AddComponent(component);
}