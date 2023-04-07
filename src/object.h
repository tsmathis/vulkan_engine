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
		glm::mat4 mat4() {
			const float c3 = glm::cos(rotation.z);
			const float s3 = glm::sin(rotation.z);
			const float c2 = glm::cos(rotation.x);
			const float s2 = glm::sin(rotation.x);
			const float c1 = glm::cos(rotation.y);
			const float s1 = glm::sin(rotation.y);

			return glm::mat4{
				{
					scale.x * (c1 * c3 + s1 * s2 * s3),
					scale.x * (c2 * s3),
					scale.x * (c1 * s2 * s3 - c3 * s1),
					0.0f
				},
				{
					scale.y * (c3 * s1 * s2 - c1 * s3),
					scale.y * (c2 * c3),
					scale.y * (c1 * c3 * s2 + s1 * s3),
					0.0f
				},
				{
					scale.z * (c2 * s1),
					scale.z * (-s2),
					scale.z * (c1 * c2),
					0.0f
				},
				{translation.x, translation.y, translation.z, 1.0f}
			};
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
		TransformComponent transform{};

	private:
		Object(id_type objID) : id(objID) {}

		id_type id;
	};
}