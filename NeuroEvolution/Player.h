#pragma once

#include "Entity.h"

class Player : public Entity
{
public:
	Player(const Vec2f& centre, float size);
	~Player();

	void Control(const Vec2f& movementVec);

	Status Update() override;

};

