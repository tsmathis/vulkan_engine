#pragma once

#include "live_window.h"
#include "live_pipeline.h"
#include "engine_device.h"
#include "engine_swap_chain.h"

#include <memory>
#include <vector>

namespace live {
	class Application {
	public:
		static const int WIDTH = 800;
		static const int HEIGHT = 600;

		Application();
		~Application();

		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;

		void run();

	private:
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void drawFrame();

		LiveWindow liveWindow{WIDTH, HEIGHT, "Hello Vulkan"};
		LiveDevice liveDevice{ liveWindow };
		LiveSwapChain liveSwapChain{ liveDevice, liveWindow.getExtent() };
		std::unique_ptr<LivePipeline> livePipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
	};
}