#include "camera.h"
#include "input/input.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <cassert>
#include <limits>



live::Camera::Camera(float vFOV, float nearClip, float farClip) : _vFOV(vFOV), _nearClip(nearClip), _farClip(farClip) {
	_forwardDirection = glm::vec3(0, 0, -1);
	_position = glm::vec3(0, 0, 3);
}

void live::Camera::onUpdate(float timeStep) {
	glm::vec2 mousePosition = Input::getMousePosition();
	glm::vec2 mouseDelta = (mousePosition - _lastMousePosition) * 0.002f;

	if (!Input::isMouseButtonDown(MouseButton::Right)) {
		Input::setCursorMode(CursorMode::Normal);
		return;
	}

	Input::setCursorMode(CursorMode::Locked);

	bool moved = false;

	constexpr glm::vec3 upDirection(0.0f, 1.0f, 0.0f);
	glm::vec3 rightDirection = glm::cross(_forwardDirection, upDirection);

	float speed = 5.0f;

	// Movement
	if (Input::isKeyDown(KeyCode::W)) {
		_position += _forwardDirection * speed * timeStep;
		moved = true;
	} else if (Input::isKeyDown(KeyCode::S)) {
		_position -= _forwardDirection * speed * timeStep;
		moved = true;
	}

	if (Input::isKeyDown(KeyCode::A)) {
		_position -= rightDirection * speed * timeStep;
		moved = true;
	}
	else if (Input::isKeyDown(KeyCode::D)) {
		_position += rightDirection * speed * timeStep;
		moved = true;
	}

	if (Input::isKeyDown(KeyCode::Q)) {
		_position -= upDirection * speed * timeStep;
		moved = true;
	}
	else if (Input::isKeyDown(KeyCode::E)) {
		_position += upDirection * speed * timeStep;
		moved = true;
	}

	// Rotation
	if (mouseDelta.x != 0.0f || mouseDelta.y != 0.0f) {
		float pitchDelta = mouseDelta.y * getRotationSpeed();
		float yawDelta = mouseDelta.x * getRotationSpeed();

		glm::quat q = glm::normalize(glm::cross(glm::angleAxis(-pitchDelta, rightDirection),
			glm::angleAxis(-yawDelta, glm::vec3(0.0F, 1.0F, 0.0F))));

		_forwardDirection = glm::rotate(q, _forwardDirection);

		moved = true;
 	}

	if (moved) {
		recalculateView();
		recalculateProjection();
	}

}

void live::Camera::onResize(uint32_t width, uint32_t height) {
	if (width == _viewportWidth && height == _viewportHeight) { return;	}

	_viewportWidth  = width;
	_viewportHeight = height;

	recalculateProjection();
	recalculateView();
}

float live::Camera::getRotationSpeed() {
	return 0.0f;
}

void live::Camera::recalculateProjection() {
	_projection = glm::perspectiveFov(glm::radians(_vFOV), (float)_viewportWidth, (float)_viewportHeight, _nearClip, _farClip);
	_inverseProjection = glm::inverse(_projection);
}

void live::Camera::recalculateView() {
	_view = glm::lookAt(_position, _position + _forwardDirection, glm::vec3(0, 1, 0));
	_inverseView = glm::inverse(_view);
}




/* Old stuff
void live::Camera::setOrthographicProjection(float left, float right, float top, float bottom, float near, float far) {
	projectionMatrix = glm::mat4{ 1.0f };
	projectionMatrix[0][0] = 2.0f / (right - left);
	projectionMatrix[1][1] = 2.0f / (bottom - top);
	projectionMatrix[2][2] = 1.0f / (far - near);
	projectionMatrix[3][0] = -(right + left) / (right - left);
	projectionMatrix[3][1] = -(bottom + top) / (bottom - top);
	projectionMatrix[3][2] = -near / (far - near);
}

void live::Camera::setPerspectiveProjection(float vFOV, float aspectRatio, float near, float far) {
	assert(glm::abs(aspectRatio - std::numeric_limits<float>::epsilon()) > 0.0f);
	const float tangentHalfvFOV = tan(vFOV / 2.0f);
	projectionMatrix = glm::mat4{ 0.0f };
	projectionMatrix[0][0] = 1.0f / (aspectRatio * tangentHalfvFOV);
	projectionMatrix[1][1] = 1.0f / (tangentHalfvFOV);
	projectionMatrix[2][2] = far / (far - near);
	projectionMatrix[2][3] = 1.0f;
	projectionMatrix[3][2] = -(far * near) / (far - near);

}

void live::Camera::setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up) {
	const glm::vec3 w{ glm::normalize(direction) };
	const glm::vec3 u{ glm::normalize(glm::cross(w, up)) };
	const glm::vec3 v{ glm::cross(w, u) };

	viewMatrix = glm::mat4{ 1.f };
	viewMatrix[0][0] = u.x;
	viewMatrix[1][0] = u.y;
	viewMatrix[2][0] = u.z;
	viewMatrix[0][1] = v.x;
	viewMatrix[1][1] = v.y;
	viewMatrix[2][1] = v.z;
	viewMatrix[0][2] = w.x;
	viewMatrix[1][2] = w.y;
	viewMatrix[2][2] = w.z;
	viewMatrix[3][0] = -glm::dot(u, position);
	viewMatrix[3][1] = -glm::dot(v, position);
	viewMatrix[3][2] = -glm::dot(w, position);
}

void live::Camera::setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up) {
	setViewDirection(position, target - position, up);
}

void live::Camera::setViewYXZ(glm::vec3 position, glm::vec3 rotation) {
	const float c3 = glm::cos(rotation.z);
	const float s3 = glm::sin(rotation.z);
	const float c2 = glm::cos(rotation.x);
	const float s2 = glm::sin(rotation.x);
	const float c1 = glm::cos(rotation.y);
	const float s1 = glm::sin(rotation.y);

	const glm::vec3 u{ (c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1) };
	const glm::vec3 v{ (c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3) };
	const glm::vec3 w{ (c2 * s1), (-s2), (c1 * c2) };

	viewMatrix = glm::mat4{ 1.f };
	viewMatrix[0][0] = u.x;
	viewMatrix[1][0] = u.y;
	viewMatrix[2][0] = u.z;
	viewMatrix[0][1] = v.x;
	viewMatrix[1][1] = v.y;
	viewMatrix[2][1] = v.z;
	viewMatrix[0][2] = w.x;
	viewMatrix[1][2] = w.y;
	viewMatrix[2][2] = w.z;
	viewMatrix[3][0] = -glm::dot(u, position);
	viewMatrix[3][1] = -glm::dot(v, position);
	viewMatrix[3][2] = -glm::dot(w, position);
}

*/