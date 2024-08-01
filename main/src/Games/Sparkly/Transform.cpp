#include "Transform.h"

	Sparkly::Transform::Transform(glm::vec3 position, glm::quat rotation, glm::vec3 size) : position(position), rotation(rotation), size(size) {}

	void Sparkly::Transform::rotate(const glm::vec3& vector, float angle) {
		glm::quat quaternion = glm::quat(glm::cos(angle/2), glm::normalize(vector) * glm::sin(angle / 2));
		rotation = quaternion * rotation;
	}

	void Sparkly::Transform::rotate(const std::vector<Rotation>& rotations) {
		for (Rotation rot : rotations) {
			glm::quat quaternion = glm::quat(glm::cos(rot.angle / 2), glm::normalize(rot.vec) * glm::sin(rot.angle / 2));
			rotation *= quaternion;
			glm::normalize(rotation);
		}
	}

	void Sparkly::Transform::rotate(glm::quat& quaternion) {
		rotation *= quaternion;
		glm::normalize(rotation);
	}

	void Sparkly::Transform::rotate(const std::vector<glm::quat>& quaternions) {
		for (glm::quat q : quaternions)
		{
			rotation *= q;
		}

		glm::normalize(rotation);
	}

	glm::mat4 Sparkly::Transform::getModelMatrix() const {
		glm::mat4 r = glm::mat4_cast(rotation);
		r[0] *= size.x;
		r[1] *= size.y;
		r[2] *= size.z;
		r[3] = glm::vec4(position, 1.0f);
		return r;
	}