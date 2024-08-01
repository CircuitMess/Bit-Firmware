#ifndef BIT_TRANSFORM_H
#define BIT_TRANSFORM_H

#include <vector>
#include <glm.hpp>
#include <gtx/quaternion.hpp>

namespace Sparkly
{
	struct Rotation {
		glm::vec3 vec;
		float angle;
	};

	class Transform
	{
	public:
		Transform(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f), glm::vec3 size = glm::vec3(1.0f, 1.0f, 1.0f));
		virtual ~Transform() = default;

		void rotate(const glm::vec3& vector, float angle);

		void rotate(const std::vector<Rotation>& rotations);

		void rotate(glm::quat& quaternion);

		void rotate(const std::vector<glm::quat>& quaternions);

		inline void translate(const glm::vec3& translation) { position = translation; }

		inline glm::vec3 getPosition() const { return position; }

		inline glm::vec3 getSize() const { return size; }

		inline glm::quat getQuat() const { return rotation; }

		inline glm::vec3 getForward() const {
			glm::mat4 r = glm::mat4_cast(rotation);
			return -glm::normalize(glm::vec3(r[2]));
		}

		inline glm::vec3 getRight() const {
			glm::mat4 r = glm::mat4_cast(rotation);
			return glm::normalize(glm::vec3(r[0]));
		}

		inline glm::vec3 getUp() const {
			glm::mat4 r = glm::mat4_cast(rotation);
			return glm::normalize(glm::vec3(r[1]));
		}

		inline void setSize(glm::vec3 value) { size = value; }

		glm::mat4 getModelMatrix() const;

	private:
		glm::vec3 position;
		glm::quat rotation;
		glm::vec3 size;
	};
}

#endif // BIT_TRANSFORM_H