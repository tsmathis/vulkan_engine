#pragma once

#include "engine_device.h"
#include "engine_swap_chain.h"
#include "live_pipeline.h"
#include "live_window.h"
#include "model.h"
#include "object.h"

#include <memory>
#include <vector>


namespace live {
	class Application {
	public:
		static constexpr int WIDTH  = 800;
		static constexpr int HEIGHT = 600;

		Application();
		~Application();

		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;

		void run();

	private:
		void loadObjects();
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void freeCommandBuffers();
		void drawFrame();
		void recreateSwapChain();
		void recordCommandBuffer(int imageIndex);
		void renderObjects(VkCommandBuffer commandBuffer);

		LiveWindow                     liveWindow{WIDTH, HEIGHT, "Hello Vulkan"};
		LiveDevice                     liveDevice{ liveWindow };
		std::unique_ptr<LiveSwapChain> liveSwapChain;
		std::unique_ptr<LivePipeline>  livePipeline;
		VkPipelineLayout               pipelineLayout;
		std::vector<VkCommandBuffer>   commandBuffers;
		std::vector<Object>            objects;
	};
}