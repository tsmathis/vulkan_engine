#pragma once

#include "model.h"

#include <memory>


namespace live {

	struct Transform2DComponent {
		glm::vec2 translation{};
		glm::vec2 scale{ 1.0f, 1.0f };
		float rotation;

		glm::mat2 mat2() {
			const float sinTheta = glm::sin(rotation);
			const float cosTheta = glm::cos(rotation);
			glm::mat2 rotationMat = { {cosTheta, sinTheta}, {-sinTheta, cosTheta} };

			glm::mat2 scaleMat{ {scale.x, 0.0f}, {0.0f, scale.y} };
			
			return rotationMat * scaleMat; 
		}
	};

	class Object {
	public:
		using id_type = unsigned int;

		static Object createObject() {
			static id_type	currentID = 0;
			return Object(currentID++);
		}

		Object(const Object&) = delete;
		Object& operator=(const Object&) = delete;

		Object(Object&&) = default;
		Object& operator=(Object&&) = default;

		const id_type getID() { return id; }

		std::shared_ptr<Model> model{};
		glm::vec3 color{};
		Transform2DComponent transform2D{};

	private:
		Object(id_type objID) : id(objID) {}

		id_type id;
	};
}