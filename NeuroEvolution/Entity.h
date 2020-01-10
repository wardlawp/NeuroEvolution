#pragma once

#include "CommonTypes.h"

class EntityComponent;
constexpr size_t k_maxComponent = 3;

class Entity
{
public:
	enum class Status {Updating, Sleeping, Dead};

	Entity();
	virtual ~Entity();

	bool IsAwake() const;
	virtual void Awake();
	void Sleep();

	template<typename T>
	T* GetComponent();
	template<typename T>
	const T* GetComponent() const;

	void AddComponent(EntityComponent*);

	//virtual void Setup() = 0;
	virtual Status Update() = 0;

private:
	std::array<EntityComponent*, k_maxComponent> m_components;
	bool m_awake;
};

using EntityList = std::vector<Entity*>;

class Wall : public Entity
{
public:
	// Note can only be orthogonal
	Wall(const Vec2f p1, const Vec2f p2, float thickness);
	void Awake() override {}; // Cannot be woken
	Status Update() override { return Status::Sleeping; }; // Always sleeps
};

class Target : public Entity
{
public:
	Target(const Vec2f p1, const float thickness);
	void Awake() override {}; // Cannot be woken
	Status Update() override { return Status::Sleeping; }; // Always sleeps
};
