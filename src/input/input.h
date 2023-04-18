#pragma once

#include "key_codes.h"

#include <glm/glm.hpp>


namespace live {
	class Input {
	public:
		static bool isKeyDown(KeyCode keyCode);
		static bool isMouseButtonDown(MouseButton mouseButton);

		static glm::vec2 getMousePosition();

		static void setCursorMode(CursorMode mode);
	};
}