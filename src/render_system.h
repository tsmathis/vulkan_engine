#pragma once

#include "camera.h"
#include "engine_device.h"
#include "live_pipeline.h"
#include "model.h"
#include "object.h"

#include <memory>
#include <vector>


namespace live {
	class RenderSystem {
	public:
		RenderSystem(LiveDevice& device, VkRenderPass renderPass);
		~RenderSystem();

		RenderSystem(const RenderSystem&) = delete;
		RenderSystem& operator=(const RenderSystem&) = delete;

		void renderObjects(VkCommandBuffer commandBuffer, std::vector<Object>& objects, const Camera& camera);

	private:
		void createPipelineLayout();
		void createPipeline(VkRenderPass renderPass);

		LiveDevice&                    device;
		std::unique_ptr<LivePipeline>  livePipeline;
		VkPipelineLayout               pipelineLayout;
	};
}