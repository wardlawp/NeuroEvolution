#include "AABB.h"

AABB::AABB(float centreX, float centreY, float width, float height)
	: m_pos{ centreX, centreY }
	, m_dim{ width, height }
{
}

void AABB::SetPos(const Vec2f pos)
{
	m_pos = pos;
}

void AABB::Translate(const Vec2f pos)
{
	m_pos = m_pos + pos;
}

const Vec2f& AABB::GetCentre() const
{
	return m_pos;
}

const Vec2f AABB::CaclBottomLeft() const
{
	return  { m_pos.x - m_dim.x / 2, m_pos.y - m_dim.y / 2 };
}

const Vec2f AABB::CalcTopRight() const
{
	return  { m_pos.x + m_dim.x / 2, m_pos.y + m_dim.y / 2 };
}

const Vec2f& AABB::GetDim() const
{
	return m_dim;
}

bool AABB::DoesIntersect(const AABB& other) const
{
	const Vec2f thisLeft = CaclBottomLeft();
	const Vec2f thisRight = CalcTopRight();
	const Vec2f otherLeft = other.CaclBottomLeft();
	const Vec2f otherRight = other.CalcTopRight();

	const bool intersects = 
		(otherLeft.x <= thisRight.x && otherLeft.y <= thisRight.y) && 
		(otherRight.x >= thisLeft.x && otherRight.y >= thisLeft.y);

	return intersects;
}

bool AABB::IsInside(const AABB& other) const
{
	const Vec2f thisLeft = CaclBottomLeft();
	const Vec2f thisRight = CalcTopRight();
	const Vec2f otherLeft = other.CaclBottomLeft();
	const Vec2f otherRight = other.CalcTopRight();

	const bool inside =
		(otherLeft.x >= thisLeft.x && otherLeft.y >= thisLeft.y) &&
		(otherRight.x <= thisRight.x && otherRight.y <= thisRight.y);

	return inside;
}