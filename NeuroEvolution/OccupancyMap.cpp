#include "OccupancyMap.h"

#include "Util.h"


OccupancyMap::OccupancyMap(const AABB& bounds)
	: m_bounds(bounds)
	, m_data(static_cast<size_t>(bounds.GetDim().x*bounds.GetDim().y), false)
{
}

void OccupancyMap::BurnOccupancy(const AABB& shape, bool burnValue)
{
	auto burnFunc = [burnValue](bool& val)
	{
		val = burnValue;
		return OccupancyMap::TraverseControl::Continue;
	};

	TraverseArea(shape, burnFunc);

}
bool OccupancyMap::TestOccupied(const AABB& shape, bool testValue) const
{
	bool result = false;

	auto testFunc = [&result, testValue](bool& val)
	{
		if (val == testValue)
		{
			result = true;
			return OccupancyMap::TraverseControl::Stop;
		}
		return OccupancyMap::TraverseControl::Continue;
	};

	const_cast<OccupancyMap&>(*this).TraverseArea(shape, testFunc);

	return result;
}
void OccupancyMap::TraverseArea(const AABB& shape, std::function<TraverseControl(bool& dataPoint)> fun)
{
	if (m_bounds.DoesIntersect(shape) == false)
	{
		return;
	}

	const size_t xDim = static_cast<size_t>(m_bounds.GetDim().x);
	const size_t yDim = static_cast<size_t>(m_bounds.GetDim().y);
	const size_t centreX = xDim / 2;
	const size_t centreY = yDim / 2;

	const Vec2f bottomLeft = shape.CaclBottomLeft();
	const Vec2f topRight = shape.CalcTopRight();

	const float xMinOffset = std::floor(bottomLeft.x - m_bounds.GetCentre().x);
	const float xOffsetClamped = clamp(xMinOffset, -static_cast<float>(centreX), +static_cast<float>(centreX -1));
	const size_t xMin = centreX + static_cast<size_t>(xOffsetClamped);
	const float yMinOffset = std::floor(bottomLeft.y - m_bounds.GetCentre().y);
	const float yOffsetClamped = clamp(yMinOffset, -static_cast<float>(centreY), +static_cast<float>(centreY - 1));
	const size_t yMin = centreY + static_cast<size_t>(yOffsetClamped);

	const float xMaxOffset = std::ceil(topRight.x - m_bounds.GetCentre().x);
	const float xMaxOffsetClamped = clamp(xMaxOffset, -static_cast<float>(centreX), +static_cast<float>(centreX));
	const size_t maxX = centreX + static_cast<size_t>(xMaxOffsetClamped);
	const float yMaxOffset = std::ceil(topRight.y - m_bounds.GetCentre().y);
	const float yMaxOffsetClamped = clamp(yMaxOffset, -static_cast<float>(centreY), +static_cast<float>(centreY));
	const size_t maxY = centreY + static_cast<size_t>(yMaxOffsetClamped);

	size_t x = xMin;
	size_t y = yMin;

	while (y < maxY)
	{

		while (x < maxX)
		{
			bool dataPoint = m_data[y*xDim + x];
			const auto control = fun(dataPoint);
			m_data[y*xDim + x] = dataPoint;

			if (control == TraverseControl::Stop)
			{
				return;
			}
			x++;
		}

		x = xMin;

		y++;
	}
}

const std::vector<bool>&  OccupancyMap::GetData() const
{
	return m_data;
}