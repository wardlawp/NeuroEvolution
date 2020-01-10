#include "Camera.h"


void Camera::Zoom(float amount)
{
	if (amount == 0.0f)
	{
		// Error
		return;
	}

	if (amount > 0)
	{
		m_dim.x /= amount;
		m_dim.y /= amount;
	}
	else
	{
		m_dim.x *= amount;
		m_dim.y *= amount;
	}
}