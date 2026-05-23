#define _USE_MATH_DEFINES
#include "mathhelper.h"


float deg2radf(float deg)
{
	static const float fac = M_PI / 180.0f;
	return deg * fac;
}

double deg2rad(double deg)
{
	static const double fac = M_PI / 180.0;
	return deg * fac;
}

float rad2degf(float rad)
{
	static const float fac = 180.0f / M_PI;
	return rad * fac;
}

double rad2deg(double rad)
{
	static const double fac = 180.0 / M_PI;
	return rad * fac;
}
