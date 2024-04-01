#pragma once
#include <iostream>

class ShadowMapPrerequisites
{
public:
	float left;
	float right;
	float bottom;
	float top;
	float zNear;
	float zFar;

	unsigned int depthMapFBO{};
	unsigned int depthMapTexture{};

	void init();

private:
	constexpr static int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
};
