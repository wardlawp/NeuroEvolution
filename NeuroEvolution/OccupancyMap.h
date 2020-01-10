#pragma once


#include <functional>

#include "AABB.h"

class OccupancyMap
{
public:
	OccupancyMap(const AABB& bounds);

	void BurnOccupancy(const AABB&, bool burnValue);
	bool TestOccupied(const AABB&, bool testValue) const;

	enum TraverseControl { Continue, Stop };
	void TraverseArea(const AABB& area, std::function<TraverseControl(bool& dataPoint)>);

	const std::vector<bool>& GetData() const;

private:
	std::vector<bool> m_data;
	AABB m_bounds;
};