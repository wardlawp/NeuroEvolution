#include "Game.h"
#include "EntityComponent.h"


Game::Game(const AABB& bounds)
	: m_entities()
	, m_player(nullptr)
	, m_bounds(bounds)
	, m_occupancyMap(bounds)
	, m_currentTick(-1)
{
}

Game::~Game()
{
	for (auto& ent : m_entities)
	{
		delete ent;
	}
}

Game::Status Game::Update()
{
	if (m_player == nullptr)
	{
		THROW("Player not set");
		return Status::Ended;
	}

	m_currentTick++;

	Game::Status retval = UpdateEntities();
	if (retval == Status::Ended) return retval;

	ResolveMovement();

	retval = CheckEndConditions();
	return retval;
}

Game::Status Game::CheckEndConditions()
{
	for (auto& endCondition : m_endConditions)
	{
		if (endCondition(*this))
		{
			return Status::Ended;
		}
	}

	return Status::Running;
}

Game::Status Game::UpdateEntities()
{

	std::vector<Entity*> entitiesToRemove;

	for (Entity* updatingEntity : m_entities)
	{
		Entity::Status entityStatus = Entity::Status::Updating;
		if (updatingEntity->IsAwake())
		{
			entityStatus = updatingEntity->Update();
		}

		if (entityStatus == Entity::Status::Dead)
		{
			if (updatingEntity == m_player)
			{
				m_player = nullptr;
				return Status::Ended;
			}

			entitiesToRemove.emplace_back(updatingEntity);
		}
	}

	auto inDeadList = [&entitiesToRemove](const Entity* e)
	{
		return std::find(entitiesToRemove.begin(), entitiesToRemove.end(), e) != entitiesToRemove.end();
	};

	const auto newEnd = std::remove_if(m_entities.begin(), m_entities.end(), inDeadList);
	m_entities.erase(newEnd, m_entities.end());

	for (auto& ent : entitiesToRemove)
	{
		BodyComponent* bComp = ent->GetComponent<BodyComponent>();

		if (bComp != nullptr && bComp->Collidable())
		{
			m_occupancyMap.BurnOccupancy(*bComp, true);
		}

		delete ent;
	}

	return Status::Running;
}

void Game::ResolveMovement()
{
	for (Entity* updatingEntity : m_entities)
	{
		BodyComponent* bComp = updatingEntity->GetComponent<BodyComponent>();

		if (bComp != nullptr && bComp->Collidable())
		{
			m_occupancyMap.BurnOccupancy(*bComp, true);
		}
	}

	for (Entity* updatingEntity : m_entities)
	{
		BodyComponent* bComp = updatingEntity->GetComponent<BodyComponent>();

		if (bComp != nullptr && bComp->MovementRequested())
		{
			const AABB dest = bComp->MovementDestination();
			const bool destIsOutOfBounds = m_bounds.IsInside(dest) == false;
			if (destIsOutOfBounds)
			{
				RUNTINE_THROW("Entity has reached edge of bounds");
				continue;
			}

			// Ignore self
			m_occupancyMap.BurnOccupancy(*bComp, false);

			if (bComp->Collidable() && m_occupancyMap.TestOccupied(dest, true))
			{
				bComp->CancelMovement();
			}
			else
			{
				bComp->CommitMovement();
			}

			// Redraw self
			m_occupancyMap.BurnOccupancy(*bComp, true);
		}
	}
}


void Game::AddEntity(Entity* e)
{
	m_entities.push_back(e);
}

void Game::AddPlayer(Player* p)
{
	m_player = p;
	m_entities.push_back(p);
}

const EntityList& Game::GetEntities() const 
{
	return m_entities;
}

Player* Game::GetPlayer()
{
	return m_player;
}

const Player* Game::GetPlayer() const
{
	return m_player;
}

void Game::AddEndContion(EndCondition e)
{
	m_endConditions.push_back(e);
}

int Game::GetCurrentTick() const
{
	return m_currentTick;
}