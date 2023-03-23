#include "live_window.h"

namespace live {

	LiveWindow::LiveWindow(int width, int height, std::string name) : width{ width }, height{ height }, windowName{ name } {
		initWindow();
	}

	LiveWindow::~LiveWindow() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void LiveWindow::initWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
	}

}