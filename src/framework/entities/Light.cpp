#include "Light.h"

Light::Light()
{
	color.set(0.10000000149011612, 0.20000000298023224, 0.40000000596046448);
	cone_info.set(25, 40);
	intensity = 1.5;
	max_distance = 1000;
	cast_shadows = true;
	shadow_bias = 0.001;
	near_distance = -500;
	area = 1000;

	shadowMapFBO = nullptr;
}