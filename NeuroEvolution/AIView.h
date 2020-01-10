#pragma once

#include <vector>

#include "AABB.h"

class Target;
class Game;

class AIView
{
public:
	AIView(const AABB& fov);

	size_t OutputSize() const;
	static size_t OutputSize(const AABB& fov);

	std::vector<float> Observe(const Game&, const Target&) const;
private:
	const AABB m_fov;
};

