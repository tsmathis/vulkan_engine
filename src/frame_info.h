#pragma once

#include "camera.h"

#include <vulkan/vulkan.h>


namespace live {
	struct FrameInfo {
		int frameIndex;
		float frameTime;
		VkCommandBuffer commandBuffer;
		Camera& camera;
	};
}