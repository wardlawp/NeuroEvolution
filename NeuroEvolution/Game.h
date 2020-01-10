#pragma once

#include <functional>

#include "CommonTypes.h"
#include "Entity.h"
#include "Player.h"
#include "AABB.h"
#include "OccupancyMap.h"


class Game;

using EndCondition = std::function<bool(const Game&)>;

class Game
{
public:

	enum class Status {Running, Ended};

	Game(const AABB& bounds);
	~Game();

	Status Update();
	
	void AddEntity(Entity*);
	void AddPlayer(Player*);
	void AddEndContion(EndCondition e);

	const EntityList& GetEntities() const;
	const Player* GetPlayer() const;
	int GetCurrentTick() const;

	Player* GetPlayer();
	
private:
	Status UpdateEntities();
	void ResolveMovement();
	Status CheckEndConditions();

	EntityList m_entities;
	Player* m_player;
	const AABB m_bounds;
	OccupancyMap m_occupancyMap;
	std::vector<EndCondition> m_endConditions;
	int m_currentTick;
};

