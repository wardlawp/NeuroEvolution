#include "AIView.h"
#include "Player.h"
#include "Game.h"
#include "EntityComponent.h"

AIView::AIView(const AABB& fov)
	: m_fov(fov)
{
}

std::vector<float> AIView::Observe(const Game& game, const Target& target) const
{
	const Player* player = game.GetPlayer();
	const BodyComponent* playerBody = player->GetComponent<BodyComponent>();
	const BodyComponent* targetBody = target.GetComponent<BodyComponent>();

	const Vec2f toTarget = targetBody->GetCentre() - playerBody->GetCentre();
	const Vec2f toTargetNorm = toTarget.Normalize();

	AABB fov = m_fov;

	fov.SetPos(playerBody->GetCentre());
	OccupancyMap wallView(fov);

	for (const Entity* ent : game.GetEntities())
	{
		if (ent == player)
		{
			continue;
		}

		const BodyComponent* entBody = ent->GetComponent<BodyComponent>();
		if (entBody != nullptr && fov.DoesIntersect(*entBody))
		{
			wallView.BurnOccupancy(*entBody, true);
		}
	}

	std::vector<float> result(wallView.GetData().size() + 2, 0.0f);

	result[0] = toTargetNorm.x;
	result[1] = toTargetNorm.y;

	// Burn obstacles as +1.0fs
	for (size_t i = 2; i < result.size(); i++)
	{
		if (wallView.GetData().at(i - 2))
		{
			result[i] = +1.0f;
		}
	}

	OccupancyMap targetView(fov);
	targetView.BurnOccupancy(*targetBody, true);

	// Burn the target as -1.0fs
	for (size_t i = 2; i < result.size(); i++)
	{
		if (targetView.GetData().at(i - 2))
		{
			result[i] = -1.0f;
		}
	}

	return result;
}

size_t AIView::OutputSize() const
{
	return AIView::OutputSize(m_fov);
}

size_t AIView::OutputSize(const AABB& fov)
{
	const Vec2f dim = fov.GetDim();

	// +2 for target direction
	const size_t result =static_cast<size_t>(dim.x) * static_cast<size_t>(dim.y) + 2;
	return result;
}