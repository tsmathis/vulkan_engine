#pragma once

#include "engine_device.h"

#include <string>
#include <vector>

namespace live {

	struct PipelineConfigInfo {};

	class LivePipeline {
	public:
		LivePipeline(
			LiveDevice& device, 
			const std::string& vertFilePath, 
			const std::string& fragFilePath, 
			const PipelineConfigInfo& configInfo);
		~LivePipeline() {}

		LivePipeline(const LivePipeline&) = delete;
		void operator=(const LivePipeline&) = delete;

		static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);

	private:
		static std::vector<char> readFile(const std::string& filePath);

		void createGraphicsPipeline(
			const std::string& vertFilePath, 
			const std::string& fragFilePath,
			const PipelineConfigInfo configInfo
		);

		void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule);

		LiveDevice&    liveDevice;
		VkPipeline     graphicsPipeline;
		VkShaderModule vertShaderModule;
		VkShaderModule fragShaderModule;
	};
}