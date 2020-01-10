#pragma once

#include "AABB.h"

class Camera: public AABB
{
public:
	// Positive values zoom in, +1.0f would lead to a reduction of FOV by 50%
	using AABB::AABB;

	void Zoom(float amount);
};

