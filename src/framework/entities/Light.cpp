#include "Light.h"

Light::Light(eLightType type, bool isDay)
{
	if (type == eLightType::DIRECTIONAL)
	{
		if (!isDay) color.set(0.10000000149011612, 0.20000000298023224, 0.40000000596046448);
		else color.set(0.95, 0.85, 0.65);
		cone_info.set(25, 40);
		intensity = 1.5;
		max_distance = 1000;
		cast_shadows = true;
		shadow_bias = 0.001;
		near_distance = -500;
		area = 1000;
	}
	if (type == eLightType::POINTLIGHT)
	{
		color.set(1, 0.20000000298023224, 0.10000000149011612);
		cone_info.set(25, 40);
		intensity = 2.5;
		max_distance = 30;
		cast_shadows = false;
		shadow_bias = 0.001;
		near_distance = 0.10000000149011612;
	}

	shadowMapFBO = nullptr;
	lightType = type;
}