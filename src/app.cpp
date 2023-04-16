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
	struct GlobalUniformBufferObj {
		glm::mat4 projectionView{ 1.0f };
		glm::vec3 lightDirection = glm::normalize(glm::vec3{ 1.0f, -3.0f, -1.0f });
	};

	Application::Application() { loadObjects();	}

	Application::~Application() {}

	void Application::run() {
		Buffer globalUniformBuffer{
			liveDevice,
			sizeof(GlobalUniformBufferObj),
			LiveSwapChain::MAX_FRAMES_IN_FLIGHT,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
			liveDevice.properties.limits.minUniformBufferOffsetAlignment
		};

		globalUniformBuffer.map();

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
				int frameIndex = renderer.getFrameINdex();
				FrameInfo frameInfo{
					frameIndex,
					frameTime,
					commandBuffer,
					camera
				};

				//Update
				GlobalUniformBufferObj uniformBufferObj{};
				uniformBufferObj.projectionView = camera.getProjectionMatrix() * camera.getViewMatrix();
				globalUniformBuffer.writeToIndex(&uniformBufferObj, frameIndex);
				globalUniformBuffer.flushIndex(frameIndex);

				// Render
				renderer.beginSwapChainRenderPass(commandBuffer);
				renderSystem.renderObjects(frameInfo, objects);
				renderer.endSwapChainRenderPass(commandBuffer);
				renderer.endFrame();
			}
		}

		vkDeviceWaitIdle(liveDevice.device());
	}

	void Application::loadObjects() {
		std::shared_ptr<Model> model = Model::createModelFromFile(liveDevice, "models/flat_vase.obj");

		auto flatVase = Object::createObject();
		flatVase.model = model;
		flatVase.transform.translation = { -0.5f, 0.5f, 2.5f };
		flatVase.transform.scale = { 1.5f, 1.5f, 1.5f };

		objects.push_back(std::move(flatVase));

		model = Model::createModelFromFile(liveDevice, "models/smooth_vase.obj");

		auto smoothVase = Object::createObject();
		smoothVase.model = model;
		smoothVase.transform.translation = { 0.5f, 0.5f, 2.5f };
		smoothVase.transform.scale = { 3.0f, 1.5f, 3.0f };

		objects.push_back(std::move(smoothVase));
	}
}