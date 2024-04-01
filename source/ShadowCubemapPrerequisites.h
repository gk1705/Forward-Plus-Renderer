#pragma once

class ShadowCubemapPrerequisites
{
public:
	float nearP;
	float farP;

	unsigned int depthMapFBO{};
	unsigned int depthMapCubemap{};

	void init();

private:
	static constexpr int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
};