#pragma once

#include "live_window.h"
#include "live_pipeline.h"

namespace live {
	class Application {
	public:
		static const int WIDTH = 800;
		static const int HEIGHT = 600;

		void run();

	private:
		LiveWindow liveWindow{WIDTH, HEIGHT, "Hello Vulkan"};
		LivePipeline livePipeline{"shaders/simple_shader.vert.spv", "shaders/simple_shader.frag.spv"};
	};
}