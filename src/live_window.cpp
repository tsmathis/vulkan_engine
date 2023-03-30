#include "live_window.h"

#include <stdexcept>

namespace live {

	LiveWindow::LiveWindow(int width, int height, std::string name) : width{ width }, height{ height }, windowName{ name } {
		initWindow();
	}

	LiveWindow::~LiveWindow() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void LiveWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
		if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
			throw std::runtime_error("Failed to create surface.");
		}
	}

	void LiveWindow::initWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, frameBufferResizeCallback);
	}

	void LiveWindow::frameBufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto resizedWindow = reinterpret_cast<LiveWindow*>(glfwGetWindowUserPointer(window));
		resizedWindow->frameBufferResized = true;
		resizedWindow->width = width;
		resizedWindow->height = height;
	}

}