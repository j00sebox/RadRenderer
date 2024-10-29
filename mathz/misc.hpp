namespace mathz {

	// degrees to radians
	static constexpr float radians(float angle)
	{
		constexpr float f = 3.14159265f / 180.f;

		return angle * f;
	}

	// radians to degrees
	static constexpr float degrees(float angle)
	{
		constexpr float f = 180.f / 3.14159265f;

		return angle * f;
	}
}
