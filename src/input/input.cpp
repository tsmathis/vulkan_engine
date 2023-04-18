#include "input.h"

#include "../app.h"

#include <GLFW/glfw3.h>


bool live::Input::isKeyDown(KeyCode keyCode) {
	GLFWwindow* windowHandle = Application::get().getWindowHandle();
	int state = glfwGetKey(windowHandle, (int)keyCode);

	return state == GLFW_PRESS || state == GLFW_REPEAT;
}

bool live::Input::isMouseButtonDown(MouseButton mouseButton) {
	GLFWwindow* windowHandle = Application::get().getWindowHandle();
	int state = glfwGetKey(windowHandle, (int)mouseButton);

	return state == GLFW_PRESS;
}

glm::vec2 live::Input::getMousePosition() {
	GLFWwindow* windowHandle = Application::get().getWindowHandle();
	
	double x, y;
	glfwGetCursorPos(windowHandle, &x, &y);
	return { (float)x, float(y) };
}

void live::Input::setCursorMode(CursorMode mode) {
	GLFWwindow* windowHandle = Application::get().getWindowHandle();
	glfwSetInputMode(windowHandle, GLFW_CURSOR, GLFW_CURSOR_NORMAL + (int)mode);
}
