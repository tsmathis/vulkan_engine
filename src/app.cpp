#include "app.h"

#define GLM_DEFINE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

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

	struct SimplePushConstantData {
		glm::mat2 transform{ 1.f };
		glm::vec2 offset;
		alignas(16) glm::vec3 color;
	};

	Application::Application() {
		loadObjects();
		createPipelineLayout();
		createPipeline();
	}

	Application::~Application() { vkDestroyPipelineLayout(liveDevice.device(), pipelineLayout, nullptr); }

	void Application::run() {
		while (!liveWindow.shouldClose()) {
			glfwPollEvents();
			
			if (auto commandBuffer = renderer.beginFrame()) {
				renderer.beginSwapChainRenderPass(commandBuffer);
				renderObjects(commandBuffer);
				renderer.endSwapChainRenderPass(commandBuffer);
				renderer.endFrame();
			}
		}

		vkDeviceWaitIdle(liveDevice.device());
	}

	void Application::loadObjects() {
		std::vector<Model::Vertex> vertices{};
		sierpinskiTri(vertices, 2, { 0.0f, -0.7f }, { 0.7f, 0.7f }, { -0.7f, 0.7f });

		std::vector<Model::Vertex> singleTri{
			{{  0.0f, -0.5f }, { 1.0f, 0.0f, 0.0f }},
			{{  0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f }},
			{{ -0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f }}
		};

		auto triModel = std::make_shared<Model>(liveDevice, singleTri);

		std::vector<glm::vec3> colors{
			{ 1.0f,  0.7f,  0.73f },
			{ 1.0f,  0.87f, 0.73f },
			{ 1.0f,  1.0f,  0.73f },
			{ 0.73f, 1.0f,  0.8f  },
			{ 0.73f, 0.88f, 1.0f  }
		};

		for (auto& color : colors) {
			color = glm::pow(color, glm::vec3{ 2.2f });
		}

		for (int i = 0; i < 40; i++){
			auto triangle = Object::createObject();
			triangle.model = triModel;
			triangle.transform2D.scale = glm::vec2(0.5f) + i * 0.025f;
			triangle.transform2D.rotation = i * 0.25f * glm::two_pi<float>();
			triangle.color = colors[i % colors.size()];

			objects.push_back(std::move(triangle));
		}
	}

	void Application::createPipelineLayout() {
		VkPushConstantRange pushConstantRange{};
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		pushConstantRange.offset     = 0;
		pushConstantRange.size       = sizeof(SimplePushConstantData);

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType                   = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount          = 0;
		pipelineLayoutInfo.pSetLayouts             = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount  = 1;
		pipelineLayoutInfo.pPushConstantRanges     = &pushConstantRange;

		if (vkCreatePipelineLayout(liveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create pipeline layout.");
		}
	}

	void Application::createPipeline() {
		assert(pipelineLayout != nullptr && "Cannot create pipeline before pipeline layout.");

		PipelineConfigInfo pipelineConfig{};
		LivePipeline::defaultPipelineConfigInfo(pipelineConfig);
		pipelineConfig.renderPass     = renderer.getSwapChainRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;

		livePipeline = std::make_unique<LivePipeline>(liveDevice, "shaders/simple_shader.vert.spv", "shaders/simple_shader.frag.spv", pipelineConfig);
	}

	void Application::renderObjects(VkCommandBuffer commandBuffer) {
		livePipeline->bind(commandBuffer);

		int i = 0;
		for (auto& obj : objects) {
			i += 1;
			obj.transform2D.rotation = glm::mod<float>(obj.transform2D.rotation + 0.001f * i, 2.0f * glm::two_pi<float>());

			SimplePushConstantData push{};
			push.offset    = obj.transform2D.translation;
			push.color     = obj.color;
			push.transform = obj.transform2D.mat2();

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