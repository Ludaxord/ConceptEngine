#pragma once

enum class CECookingResult {
	Success,
	ZeroAreaTestFailed,
	PolygonLimitReached,
	LargeTriangle,
	Failure
};

enum class CEForceMode: uint8 {
	Force = 0,
	Impulse,
	VelocityChange,
	Acceleration
};

enum class CEActorLockFlag {
	PositionX = BIT(0),
	PositionY = BIT(1),
	PositionZ = BIT(2),
	RotationX = BIT(3),
	RotationY = BIT(4),
	RotationZ = BIT(5)
};
