#pragma once

enum class RenderPass : unsigned int {
	MainGeometry = 0,
	Lighting = 1,
	PostProcessing = 2,
	UI = 3,
};
