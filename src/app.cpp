#include "app.h"

#include "camera.h"
#include "keyboard_input.h"
#include "render_system.h"

#define GLM_DEFINE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <array>
#include <chrono>
#include <stdexcept>


namespace live {
	Application::Application() { loadObjects();	}

	Application::~Application() {}

	void Application::run() {
		RenderSystem renderSystem{liveDevice, renderer.getSwapChainRenderPass()};
		Camera camera{};
		camera.setViewTarget(glm::vec3(-1.0f, -2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 2.5f));

		auto viewerObject = Object::createObject();
		KeyboardInputController cameraController{};

		auto currentTime = std::chrono::high_resolution_clock::now();

		while (!liveWindow.shouldClose()) {
			glfwPollEvents();

			auto newTime = std::chrono::high_resolution_clock::now();
			float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
			currentTime = newTime;

			cameraController.moveInPlaneXZ(liveWindow.getGLFWwindow(), frameTime, viewerObject);
			camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

			float aspect = renderer.getAspectRatio();
			camera.setPerspectiveProjection(glm::radians(50.0f), aspect, 0.1f, 10.0f);
			
			if (auto commandBuffer = renderer.beginFrame()) {
				renderer.beginSwapChainRenderPass(commandBuffer);
				renderSystem.renderObjects(commandBuffer, objects, camera);
				renderer.endSwapChainRenderPass(commandBuffer);
				renderer.endFrame();
			}
		}

		vkDeviceWaitIdle(liveDevice.device());
	}

	void Application::loadObjects() {
		std::shared_ptr<Model> model = Model::createModelFromFile(liveDevice, "models/flat_vase.obj");

		auto loadedModel = Object::createObject();
		loadedModel.model = model;
		loadedModel.transform.translation = { 0.0f, 0.5f, 2.5f };
		loadedModel.transform.scale = { 1.5f, 1.5f, 1.5f };

		objects.push_back(std::move(loadedModel));
	}
}