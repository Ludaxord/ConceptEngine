#pragma once

class CEPhysicsMaterial {
public:
	float StaticFriction;
	float DynamicFriction;
	float Bounciness;

	CEPhysicsMaterial() = default;
	    ~CEPhysicsMaterial() = default;

	CEPhysicsMaterial(float InStaticFriction, float InDynamicFriction, float InBounciness) :
		StaticFriction(InStaticFriction), DynamicFriction(InDynamicFriction), Bounciness(InBounciness) {

	}
};
