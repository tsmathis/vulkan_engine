#include "render_system.h"

#define GLM_DEFINE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <array>
#include <stdexcept>


namespace live {

	struct SimplePushConstantData {
		glm::mat4 transform{ 1.f };
		alignas(16) glm::vec3 color;
	};

	RenderSystem::RenderSystem(LiveDevice& device, VkRenderPass renderPass) : device{ device } {
		createPipelineLayout();
		createPipeline(renderPass);
	}

	RenderSystem::~RenderSystem() { vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr); }

	void RenderSystem::createPipelineLayout() {
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(SimplePushConstantData);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create pipeline layout.");
		}
	}

	void RenderSystem::createPipeline(VkRenderPass renderPass) {
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout.");

		PipelineConfigInfo pipelineConfig{};
		LivePipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass = renderPass;
		pipelineConfig.pipelineLayout = pipelineLayout;

		livePipeline = std::make_unique<LivePipeline>(device, "shaders/simple_shader.vert.spv", "shaders/simple_shader.frag.spv", pipelineConfig);
	}

	void RenderSystem::renderObjects(VkCommandBuffer commandBuffer, std::vector<Object>& objects) {
		livePipeline->bind(commandBuffer);

		int i = 0;
		for (auto& obj : objects) {
			i += 1;
			obj.transform.rotation.y = glm::mod<float>(obj.transform.rotation.y + 0.01f * i, 2.0f * glm::two_pi<float>());
			obj.transform.rotation.x = glm::mod<float>(obj.transform.rotation.x + 0.005f * i, 2.0f * glm::two_pi<float>());

			SimplePushConstantData push{};
			push.color = obj.color;
			push.transform = obj.transform.mat4();

			vkCmdPushConstants(
				commandBuffer,
				pipelineLayout,
				VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
				0,
				sizeof(SimplePushConstantData),
				&push
			);

			obj.model->bind(commandBuffer);
			obj.model->draw(commandBuffer);
		}
	}
}