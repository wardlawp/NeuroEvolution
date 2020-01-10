#pragma once

#include "CommonTypes.h"
#include "AABB.h"

class EntityComponent
{
public:
	EntityComponent();
	virtual ~EntityComponent();
};

class HealthComponent : public EntityComponent
{
public:
	HealthComponent(short maxHealth);
	bool IsAlive() const;
	void ModifyVal(short);

private:
	short m_health;
};

class BodyComponent : public EntityComponent, public AABB
{
public:
	BodyComponent(float centreX, float centreY, float width, float height, Colour c, bool collidable = true);
	const Colour GetColour() const;

	bool MovementRequested() const;
	bool Collidable() const;
	void CancelMovement();
	void CommitMovement();
	void RequestMovement(const Vec2f& movementVector);

	AABB MovementDestination() const;

private:
	const Colour m_colour;
	Vec2f m_movementRequest;
	bool m_collidable;
};