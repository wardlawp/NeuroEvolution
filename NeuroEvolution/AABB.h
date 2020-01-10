#pragma once

#include "CommonTypes.h"

class AABB
{
public:
	AABB(float centreX, float centreY, float width, float height);

	void SetPos(const Vec2f);
	void Translate(const Vec2f);
	const Vec2f& GetCentre() const;
	const Vec2f CaclBottomLeft() const;
	const Vec2f CalcTopRight()  const;
	const Vec2f& GetDim() const;
	bool DoesIntersect(const AABB& other) const;
	// Is other inside this
	bool IsInside(const AABB& other) const;
protected:
	Vec2f m_pos;
	Vec2f m_dim;
};