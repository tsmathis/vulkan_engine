#pragma once

#include "model.h"

#include <glm/gtc/matrix_transform.hpp>

#include <memory>


namespace live {

	struct TransformComponent {
		glm::vec3 translation{};
		glm::vec3 scale{ 1.0f, 1.0f, 1.0f };
		glm::vec3 rotation;

		// Matrix rotation uses Tait-Bryan convention with axis order y(1), x(2), z(3)
		glm::mat4 mat4();
		glm::mat3 normalMatrix();
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
		TransformComponent transform{};

	private:
		Object(id_type objID) : id(objID) {}

		id_type id;
	};
}