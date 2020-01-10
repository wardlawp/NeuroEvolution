#include "Player.h"

#include"EntityComponent.h"


Player::Player(const Vec2f& centre, float size)
{
	if (size <= 0.0f)
	{
		THROW("Player size should be greater than zero");
		return;
	}

	BodyComponent* component = new BodyComponent(centre.x, centre.y, size, size, Colour::Red);
	AddComponent(component);
}


Player::~Player()
{
}


Entity::Status Player::Update()
{
	return Status::Updating;
}

void Player::Control(const Vec2f& movementVec)
{
	GetComponent<BodyComponent>()->RequestMovement(movementVec);
}

