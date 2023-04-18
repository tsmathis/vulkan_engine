#pragma once

#include "engine_device.h"
#include "live_window.h"
#include "model.h"
#include "object.h"
#include "renderer.h"

#include <memory>
#include <vector>


namespace live {
	class Application {
	public:
		static constexpr int WIDTH  = 800;
		static constexpr int HEIGHT = 600;

		Application();
		~Application();

		static Application& get();
		GLFWwindow* getWindowHandle() const { return liveWindow.getGLFWwindow(); }

		Application(const Application&) = delete;
		Application& operator=(const Application&) = delete;

		void run();

	private:
		void loadObjects();

		LiveWindow          liveWindow{WIDTH, HEIGHT, "Hello Vulkan"};
		LiveDevice          liveDevice{ liveWindow };
		Renderer            renderer{ liveWindow, liveDevice };
		std::vector<Object> objects;
	};
}