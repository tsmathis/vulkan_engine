#include "app.h"

namespace live {
	void Application::run() {
		while (!liveWindow.shouldClose()) {
			glfwPollEvents();
		}
	}
}