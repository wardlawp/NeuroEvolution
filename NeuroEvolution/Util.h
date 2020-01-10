#pragma once

template<class T>
constexpr const T& clamp(const T& v, const T& lo, const T& hi)
{
	return (v < lo) ? lo : (hi < v) ? hi : v;
}

#define RUNTINE_THROW(msg) throw std::runtime_error(msg);

#ifdef DEBUG
#define THROW(msg) throw std::runtime_error(msg);
#define ASSERT(expr, msg) if(!(expr)) { THROW(msg) }
#else
#define  THROW(msg)
#define  ASSERT(expr, msg)
#endif 



