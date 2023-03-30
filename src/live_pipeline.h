#pragma once

#include "engine_device.h"

#include <string>
#include <vector>

namespace live {

	struct PipelineConfigInfo {
		PipelineConfigInfo() = default;
		PipelineConfigInfo(const PipelineConfigInfo&) = delete;
		PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

		VkPipelineViewportStateCreateInfo      viewportInfo;
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo   multisampleInfo;
		VkPipelineColorBlendAttachmentState    colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo    colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo  depthStencilInfo;
		std::vector<VkDynamicState>            dynamicStateEnables;
		VkPipelineDynamicStateCreateInfo       dynamicStateInfo;
		VkPipelineLayout                       pipelineLayout = nullptr;
		VkRenderPass                           renderPass = nullptr;
		uint32_t                               subpass = 0;
	};


	class LivePipeline {
	public:
		LivePipeline(
			LiveDevice& device, 
			const std::string& vertFilePath, 
			const std::string& fragFilePath, 
			const PipelineConfigInfo& configInfo);
		
		~LivePipeline();

		LivePipeline(const LivePipeline&) = delete;
		LivePipeline &operator=(const LivePipeline&) = delete;

		void bind(VkCommandBuffer commandBuffer);
		static void defaultPipelineConfigInfo(PipelineConfigInfo& configInfo);

	private:
		static std::vector<char> readFile(const std::string& filePath);

		void createGraphicsPipeline(
			const std::string& vertFilePath, 
			const std::string& fragFilePath,
			const PipelineConfigInfo& configInfo
		);

		void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

		LiveDevice&    liveDevice;
		VkPipeline     graphicsPipeline;
		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;
	};
}