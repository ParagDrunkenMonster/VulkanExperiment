#ifndef __Camera_h__
#define __Camera_h__

#include <glm/glm.hpp>

namespace VulkanTutorial
{
	class Camera
	{
	public:

		Camera();
		virtual ~Camera();

		Camera(const Camera&) = delete;
		Camera& operator = (const Camera&) = delete;

		Camera(Camera&&) = delete;
		Camera& operator = (Camera&&) = delete;

		void SetOrthographicsProjection(float Left, float Right, float Top, float Bottom, float Near, float Far);
		void SetPerspectiveProjection(float FovY, float Aspect, float Near, float Far);

		void SetViewDirection(glm::vec3 Pos, glm::vec3 Dir, glm::vec3 Up = glm::vec3{ 0.0f, -1.0f, 0.0f });
		void SetViewTarget(glm::vec3 Pos, glm::vec3 Target, glm::vec3 Up = glm::vec3{ 0.0f, -1.0f, 0.0f });
		void SetViewYXZ(glm::vec3 Pos, glm::vec3 Rot);

		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
	private:

		glm::mat4 m_ProjectionMatrix{1.0f};
		glm::mat4 m_ViewMatrix{ 1.0f };
	};
}

#endif //__Camera_h__
