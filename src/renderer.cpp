#include "renderer.h"

#include <array>
#include <stdexcept>


namespace live {
	Renderer::Renderer(LiveWindow& window, LiveDevice& device) : window{ window }, device{ device } {
		recreateSwapChain();
		createCommandBuffers();
	}

	Renderer::~Renderer() { freeCommandBuffers(); }

	void Renderer::recreateSwapChain() {
		auto extent = window.getExtent();
		while (extent.width == 0 || extent.height == 0) {
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(device.device());

		if (liveSwapChain == nullptr) {
			liveSwapChain = std::make_unique<LiveSwapChain>(device, extent);
		}
		else {
			liveSwapChain = std::make_unique<LiveSwapChain>(device, extent, std::move(liveSwapChain));

			if (liveSwapChain->imageCount() != commandBuffers.size()) {
				freeCommandBuffers();
				createCommandBuffers();
			}
		}

	}

	void Renderer::createCommandBuffers() {
		commandBuffers.resize(liveSwapChain->imageCount());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = device.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(device.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate command buffers");
		}
	}

	void Renderer::freeCommandBuffers() {
		vkFreeCommandBuffers(device.device(), device.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

		commandBuffers.clear();
	}

	VkCommandBuffer Renderer::beginFrame() {
		assert(!frameStarted && "Cannot call beginFrame while already in progress");

		auto result = liveSwapChain->acquireNextImage(&currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("Failed to acquire swapchain image.");
		}

		frameStarted = true;

		auto commandBuffer = getCurrentCommandBuffer();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("Failed to begin recording command buffer.");
		}

		return commandBuffer;
	}

	void Renderer::endFrame() {
		assert(frameStarted && "Cannot call endFrame while frame is in progress");

		auto commandBuffer = getCurrentCommandBuffer();
		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("Failed to record command buffer.");
		}

		auto result = liveSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.windowResized()) {
			window.resetWindowResizedFlag();
			recreateSwapChain();
		} else if (result != VK_SUCCESS) {
			throw std::runtime_error("Failed to present swapchain image.");
		}

		frameStarted = false;
	}

	void Renderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(frameStarted && "Cannot call beginSwapChainRenderPass while frame is in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "Cannot begin render pass on command buffer from a different frame");

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.01f, 0.01f, 0.01f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = liveSwapChain->getRenderPass();
		renderPassInfo.framebuffer = liveSwapChain->getFrameBuffer(currentImageIndex);
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = liveSwapChain->getSwapChainExtent();
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(liveSwapChain->getSwapChainExtent().width);
		viewport.height = static_cast<float>(liveSwapChain->getSwapChainExtent().height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor{ {0, 0}, liveSwapChain->getSwapChainExtent() };
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);		
	}

	void Renderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer) {
		assert(frameStarted && "Cannot call endSwapChainRenderPass while frame is in progress");
		assert(commandBuffer == getCurrentCommandBuffer() && "Cannot end render pass on command buffer from a different frame");

		vkCmdEndRenderPass(commandBuffer);
	}
}