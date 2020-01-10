#pragma once

#include <array>
#include <vector>
#include <cmath>

#include "Util.h"

enum class Colour : short { Red = 0, Blue, Yellow, Green, Grey, Black, COUNT };

struct Vec2f
{

	float x = 0.0f;
	float y = 0.0f;

	static const Vec2f& Origin()
	{
		static Vec2f val{0.0f, 0.0f};
		return val;
	}

	Vec2f operator+(const Vec2f& other) const
	{
		return { x + other.x, y + other.y };
	}

	Vec2f operator-(const Vec2f& other) const
	{
		return { x - other.x, y - other.y };
	}

	Vec2f Normalize() const
	{
		const float dist = Distance();
		if (dist == 0.0f)
		{
			THROW("Normalizing bad vector");
			
			return Vec2f::Origin();
		}

		return { x / dist, y / dist };
	}

	float Distance() const
	{
		float distSqrd = x * x + y * y;

		if (distSqrd == 0.0f)
		{
			return 0.0f;
		}

		float dist = std::sqrtf(distSqrd);
		return dist;
	}

};

