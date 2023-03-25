#include "live_pipeline.h"

#include <fstream>
#include <iostream>
#include <stdexcept>


live::LivePipeline::LivePipeline(
	LiveDevice& device, 
	const std::string& vertFilePath, 
	const std::string& fragFilePath, 
	const PipelineConfigInfo& configInfo)
	: liveDevice{ device } {
	createGraphicsPipeline(vertFilePath, fragFilePath, configInfo);
}


live::PipelineConfigInfo live::LivePipeline::defaultPipelineConfigInfo(uint32_t width, uint32_t height) {
	PipelineConfigInfo configInfo{};
	configInfo.inputAssemblyInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	configInfo.inputAssemblyInfo.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	configInfo.inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

	configInfo.viewport.x        = 0.0f;
	configInfo.viewport.y        = 0.0f;
	configInfo.viewport.width    = static_cast<float>(width);
	configInfo.viewport.height   = static_cast<float>(height);
	configInfo.viewport.minDepth = 0.0f;
	configInfo.viewport.maxDepth = 1.0f;

	configInfo.scissor.offset = { 0, 0 };
	configInfo.scissor.extent = { width, height };

	configInfo.viewportInfo.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	configInfo.viewportInfo.viewportCount = 1;
	configInfo.viewportInfo.pViewports    = &configInfo.viewport;
	configInfo.viewportInfo.scissorCount  = 1;
	configInfo.viewportInfo.pScissors     = &configInfo.scissor;

	configInfo.rasterizationInfo.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	configInfo.rasterizationInfo.depthClampEnable        = VK_FALSE;
	configInfo.rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
	configInfo.rasterizationInfo.polygonMode             = VK_POLYGON_MODE_FILL;
	configInfo.rasterizationInfo.lineWidth               = 1.0f;
	configInfo.rasterizationInfo.cullMode                = VK_CULL_MODE_NONE;
	configInfo.rasterizationInfo.frontFace               = VK_FRONT_FACE_CLOCKWISE;
	configInfo.rasterizationInfo.depthBiasEnable         = VK_FALSE;
	configInfo.rasterizationInfo.depthBiasConstantFactor = 0.0f;  // Optional
	configInfo.rasterizationInfo.depthBiasClamp          = 0.0f;  // Optional
	configInfo.rasterizationInfo.depthBiasSlopeFactor    = 0.0f;  // Optional

	configInfo.multisampleInfo.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	configInfo.multisampleInfo.sampleShadingEnable   = VK_FALSE;
	configInfo.multisampleInfo.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
	configInfo.multisampleInfo.minSampleShading      = 1.0f;      // Optional
	configInfo.multisampleInfo.pSampleMask           = nullptr;   // Optional
	configInfo.multisampleInfo.alphaToCoverageEnable = VK_FALSE;  // Optional
	configInfo.multisampleInfo.alphaToOneEnable      = VK_FALSE;  // Optional

	configInfo.colorBlendAttachment.colorWriteMask =
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_A_BIT;
	configInfo.colorBlendAttachment.blendEnable         = VK_FALSE;
	configInfo.colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
	configInfo.colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
	configInfo.colorBlendAttachment.colorBlendOp        = VK_BLEND_OP_ADD;
	configInfo.colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	configInfo.colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	configInfo.colorBlendAttachment.alphaBlendOp        = VK_BLEND_OP_ADD;

	configInfo.colorBlendInfo.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	configInfo.colorBlendInfo.logicOpEnable     = VK_FALSE;
	configInfo.colorBlendInfo.logicOp           = VK_LOGIC_OP_COPY;
	configInfo.colorBlendInfo.attachmentCount   = 1;
	configInfo.colorBlendInfo.pAttachments      = &configInfo.colorBlendAttachment;
	configInfo.colorBlendInfo.blendConstants[0] = 0.0f;  // Optional
	configInfo.colorBlendInfo.blendConstants[1] = 0.0f;  // Optional 
	configInfo.colorBlendInfo.blendConstants[2] = 0.0f;  // Optional
	configInfo.colorBlendInfo.blendConstants[3] = 0.0f;  // Optional

	configInfo.depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	configInfo.depthStencilInfo.depthTestEnable       = VK_TRUE;
	configInfo.depthStencilInfo.depthWriteEnable      = VK_TRUE;
	configInfo.depthStencilInfo.depthCompareOp        = VK_COMPARE_OP_LESS;
	configInfo.depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
	configInfo.depthStencilInfo.minDepthBounds        = 0.0f;  // Optional
	configInfo.depthStencilInfo.maxDepthBounds        = 1.0f;  // Optional
	configInfo.depthStencilInfo.stencilTestEnable     = VK_FALSE;
	configInfo.depthStencilInfo.front                 = {};    // Optional
	configInfo.depthStencilInfo.back                  = {};    // Optional

	return configInfo;
}

std::vector<char> live::LivePipeline::readFile(const std::string& filePath) {
	std::ifstream file(filePath, std::ios::ate | std::ios::binary);
	
	if (!file.is_open()) {
		throw std::runtime_error("Failed to open file: " + filePath);
	}

	size_t fileSize = static_cast<size_t>(file.tellg());
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();
	
	return buffer;
}

void live::LivePipeline::createGraphicsPipeline(const std::string& vertFilePath, const std::string& fragFilePath, const PipelineConfigInfo configInfo) {
	auto vertCode = readFile(vertFilePath);
	auto fragCode = readFile(fragFilePath);

	std::cout << "Vertex Shader Code size: " << vertCode.size() << '\n';
	std::cout << "Fragement Shader Code size: " << fragCode.size() << '\n';
}

void live::LivePipeline::createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule) {
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode    = reinterpret_cast<const uint32_t*>(code.data());

	if (vkCreateShaderModule(liveDevice.device(), &createInfo, nullptr, shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create shader module");
	}
}
