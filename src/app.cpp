#include "app.h"
#include "render_system.h"

#define GLM_DEFINE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <array>
#include <stdexcept>


//void sierpinskiTri(std::vector<live::Model::Vertex>& vertices, int depth, glm::vec2 top, glm::vec2 right, glm::vec2 left) {
//	if (depth <= 0) {
//		vertices.push_back({ top,   { 1.0f, 0.0f, 0.0f } });
//		vertices.push_back({ right, { 0.0f, 1.0f, 0.0f } });
//		vertices.push_back({ left,  { 0.0f, 0.0f, 1.0f } });
//	} else {
//		auto leftTop  = 0.5f * (left + top);
//		auto rightTop = 0.5f * (right + top);
//		auto bottom   = 0.5f * (left + right);
//
//		sierpinskiTri(vertices, depth - 1, top, rightTop, leftTop);
//		sierpinskiTri(vertices, depth - 1, rightTop, right, bottom);
//		sierpinskiTri(vertices, depth - 1, leftTop, bottom, left);
//	}
//}


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

	// temporary helper function, creates a 1x1x1 cube centered at offset
	std::unique_ptr<Model> createCubeModel(LiveDevice& device, glm::vec3 offset) {
		std::vector<Model::Vertex> vertices{

			// left face (white)
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},

			// right face (yellow)
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},

			// top face (orange, remember y axis points down)
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},

			// bottom face (red)
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},

			// nose face (blue)
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},

			// tail face (green)
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},

		};
		for (auto& v : vertices) {
			v.position += offset;
		}
		return std::make_unique<Model>(device, vertices);
	}

	void Application::loadObjects() {
		std::shared_ptr<Model> model = createCubeModel(liveDevice, { 0.0f, 0.0f, 0.0f });

		auto cube = Object::createObject();
		cube.model = model;
		cube.transform.translation = { 0.0f, 0.0f, 0.5f };
		cube.transform.scale = { 0.5f, 0.5f, 0.5f };

		objects.push_back(std::move(cube));
	}
}