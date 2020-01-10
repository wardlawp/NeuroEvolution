#include "EntityComponent.h"

EntityComponent::EntityComponent()
{
}

EntityComponent::~EntityComponent()
{
}

HealthComponent::HealthComponent(short maxHealth) : m_health(maxHealth) 
{
}

bool HealthComponent::IsAlive() const
{
	return m_health > 0;
}

void HealthComponent::ModifyVal(short diff)
{
	m_health += diff;
}

BodyComponent::BodyComponent(float centreX, float centreY, float width, float height, Colour c, bool collidable)
	: AABB(centreX, centreY, width, height)
	, m_colour(c)
	, m_movementRequest{ 0.0f, 0.0f }
	, m_collidable(collidable)
{
}

const Colour BodyComponent::GetColour() const
{
	return m_colour;
}

bool BodyComponent::MovementRequested() const
{
	const bool result = m_movementRequest.Distance() > 0.0f;
	return result;
}

void BodyComponent::CancelMovement()
{
	m_movementRequest = Vec2f::Origin();
}

void BodyComponent::CommitMovement()
{
	Translate(m_movementRequest);
	m_movementRequest = Vec2f::Origin();
}

void BodyComponent::RequestMovement(const Vec2f& movementVector)
{
	m_movementRequest = movementVector;
}

AABB BodyComponent::MovementDestination() const
{
	AABB copy = *this;
	copy.Translate(m_movementRequest);

	return copy;
}

bool BodyComponent::Collidable() const
{
	return m_collidable;
}
