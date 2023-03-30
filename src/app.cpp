#include "app.h"

#include <array>
#include <stdexcept>


void sierpinskiTri(std::vector<live::Model::Vertex>& vertices, int depth, glm::vec2 top, glm::vec2 right, glm::vec2 left) {
	if (depth <= 0) {
		vertices.push_back({ top,   { 1.0f, 0.0f, 0.0f } });
		vertices.push_back({ right, { 0.0f, 1.0f, 0.0f } });
		vertices.push_back({ left,  { 0.0f, 0.0f, 1.0f } });
	} else {
		auto leftTop  = 0.5f * (left + top);
		auto rightTop = 0.5f * (right + top);
		auto bottom   = 0.5f * (left + right);

		sierpinskiTri(vertices, depth - 1, top, rightTop, leftTop);
		sierpinskiTri(vertices, depth - 1, rightTop, right, bottom);
		sierpinskiTri(vertices, depth - 1, leftTop, bottom, left);
	}
}


namespace live {
	Application::Application() {
		loadModels();
		createPipelineLayout();
		recreateSwapChain();
		createCommandBuffers();
	}

	Application::~Application() { vkDestroyPipelineLayout(liveDevice.device(), pipelineLayout, nullptr); }

	void Application::run() {
		while (!liveWindow.shouldClose()) {
			glfwPollEvents();
			drawFrame();
		}

		vkDeviceWaitIdle(liveDevice.device());
	}

	void Application::loadModels() {
		std::vector<Model::Vertex> vertices{};

		sierpinskiTri(vertices, 2, {0.0f, -0.7f}, {0.7f, 0.7f}, {-0.7f, 0.7f});

		model = std::make_unique<Model>(liveDevice, vertices);
	}

	void Application::createPipelineLayout() {
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType                   = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount          = 0;
		pipelineLayoutInfo.pSetLayouts             = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount  = 0;
		pipelineLayoutInfo.pPushConstantRanges     = nullptr;

		if (vkCreatePipelineLayout(liveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create pipeline layout.");
		}
	}

	void Application::createPipeline() {
		assert(liveSwapChain != nullptr && "Cannot create pipeline before swap chain.");
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout.");

		PipelineConfigInfo pipelineConfig{};
		LivePipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass     = liveSwapChain->getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;

		livePipeline = std::make_unique<LivePipeline>(liveDevice, "shaders/simple_shader.vert.spv", "shaders/simple_shader.frag.spv", pipelineConfig);
	}

	void Application::recreateSwapChain() {
		auto extent = liveWindow.getExtent();
		while (extent.width == 0 || extent.height == 0) {
			extent = liveWindow.getExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(liveDevice.device());
		
		if (liveSwapChain == nullptr) {
			liveSwapChain = std::make_unique<LiveSwapChain>(liveDevice, extent);
		} else {
			liveSwapChain = std::make_unique<LiveSwapChain>(liveDevice, extent, std::move(liveSwapChain));

			if (liveSwapChain->imageCount() != commandBuffers.size()) {
				freeCommandBuffers();
				createCommandBuffers();
			}
		}
		
		createPipeline();
	}

	void Application::createCommandBuffers() {
		commandBuffers.resize(liveSwapChain->imageCount());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool        = liveDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(liveDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate command buffers");
		}
	}

	void Application::freeCommandBuffers() {
		vkFreeCommandBuffers(liveDevice.device(), liveDevice.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

		commandBuffers.clear();
	}

	void Application::recordCommandBuffer(int imageIndex) {
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("Failed to begin recording command buffer.");
		}

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass        = liveSwapChain->getRenderPass();
		renderPassInfo.framebuffer       = liveSwapChain->getFrameBuffer(imageIndex);
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = liveSwapChain->getSwapChainExtent();
		renderPassInfo.clearValueCount   = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues      = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x        = 0.0f;
		viewport.y        = 0.0f;
		viewport.width    = static_cast<float>(liveSwapChain->getSwapChainExtent().width);
		viewport.height   = static_cast<float>(liveSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);

		VkRect2D scissor{ {0, 0}, liveSwapChain->getSwapChainExtent() };
		vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

		livePipeline->bind(commandBuffers[imageIndex]);
		model->bind(commandBuffers[imageIndex]);
		model->draw(commandBuffers[imageIndex]);

		vkCmdEndRenderPass(commandBuffers[imageIndex]);
		if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
			throw std::runtime_error("Failed to record command buffer.");
		}
	}

	void Application::drawFrame() {
		uint32_t imageIndex;
		auto result = liveSwapChain->acquireNextImage(&imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("Failed to acquire swapchain image.");
		}

		recordCommandBuffer(imageIndex);
		result = liveSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || liveWindow.windowResized()) {
			liveWindow.resetWindowResizedFlag();
			recreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to present swapchain image.");
		}
	}
}