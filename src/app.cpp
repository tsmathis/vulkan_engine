#include "app.h"
#include "render_system.h"

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
	Application::Application() { loadObjects();	}

	Application::~Application() {}

	void Application::run() {
		RenderSystem renderSystem{liveDevice, renderer.getSwapChainRenderPass()};

		while (!liveWindow.shouldClose()) {
			glfwPollEvents();
			
			if (auto commandBuffer = renderer.beginFrame()) {
				renderer.beginSwapChainRenderPass(commandBuffer);
				renderSystem.renderObjects(commandBuffer, objects);
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
}