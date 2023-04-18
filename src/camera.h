#pragma once

//#define GLM_DEFINE_RADIANS
//#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>


namespace live {
	class Camera {
	public:
		Camera(float vFOV, float nearClip, float farClip);

		void onUpdate(float timeStep);
		void onResize(uint32_t width, uint32_t height);

		const glm::mat4& getProjection() const { return _projection; }
		const glm::mat4& getInverseProjection() const { return _inverseProjection; }
		const glm::mat4& getView() const { return _view; }
		const glm::mat4& getInverseView() const { return _inverseView; }

		const glm::vec3& getPostion() const { return _position; }
		const glm::vec3& getDirection() const { return _forwardDirection; }

		float getRotationSpeed();

	private:
		void recalculateProjection();
		void recalculateView();

		/* Old stuff
		void setOrthographicProjection(float left, float right, float top, float bottom, float near, float far);
		void setPerspectiveProjection(float vFOV, float aspectRatio, float near, float far);

		void setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up = glm::vec3{ 0.0f, -1.0f, 0.0f });
		void setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up = glm::vec3{ 0.0f, -1.0f, 0.0f });
		void setViewYXZ(glm::vec3 position, glm::vec3 rotation);

		const glm::mat4& getProjectionMatrix() const { return projectionMatrix; }
		const glm::mat4& getViewMatrix() const { return viewMatrix; }
		*/


	private:
		glm::mat4 _projection{ 1.0f };
		glm::mat4 _view{ 1.0f };
		glm::mat4 _inverseProjection{ 1.0f };
		glm::mat4 _inverseView{ 1.0f };

		float _vFOV = 45.0f;
		float _nearClip = 0.1f;
		float _farClip = 100.0f;

		glm::vec3 _position{ 0.0f, 0.0f, 0.0f };
		glm::vec3 _forwardDirection{ 0.0f, 0.0f, 0.0f };

		glm::vec2 _lastMousePosition{ 0.0f, 0.0f };

		uint32_t _viewportWidth = 0;
		uint32_t _viewportHeight = 0;

		/* Old stuff
		glm::mat4 projectionMatrix{ 1.0f };
		glm::mat4 viewMatrix{ 1.0f };
		*/
	};
}