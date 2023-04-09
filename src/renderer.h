#pragma once

#include "engine_device.h"
#include "engine_swap_chain.h"
#include "live_window.h"
#include "model.h"

#include <cassert>
#include <memory>
#include <vector>


namespace live {
	class Renderer {
	public:
		Renderer(LiveWindow& window, LiveDevice& device);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

		VkCommandBuffer beginFrame();
		void endFrame();
		void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
		void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

		VkRenderPass getSwapChainRenderPass() const { return liveSwapChain->getRenderPass(); }
		float getAspectRatio() const { return liveSwapChain->extentAspectRatio(); }
		bool frameInProgess() const { return frameStarted; }

		VkCommandBuffer getCurrentCommandBuffer() const { 
			assert(frameStarted && "Cannot get command buffer when frame is not in progress");
			return commandBuffers[currentFrameIndex]; 
		}

		int getFrameINdex() const {
			assert(frameStarted && "Cannot get frame index when frame is not in progress");
			return currentFrameIndex;
		}

	private:
		void createCommandBuffers();
		void freeCommandBuffers();
		void recreateSwapChain();
		
		LiveWindow&                    window;
		LiveDevice&                    device;
		std::unique_ptr<LiveSwapChain> liveSwapChain;
		std::vector<VkCommandBuffer>   commandBuffers;
		uint32_t                       currentImageIndex;
		int                            currentFrameIndex{ 0 };
		bool                           frameStarted{ false };
	};
}