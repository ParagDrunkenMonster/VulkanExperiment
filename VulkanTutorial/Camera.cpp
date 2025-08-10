#include "Camera.h"
#include <cassert>

namespace VulkanTutorial
{
	Camera::Camera()
	{

	}

	Camera::~Camera()
	{

	}

	void Camera::SetOrthographicsProjection(float Left, float Right, float Top, float Bottom, float Near, float Far)
	{
		m_ProjectionMatrix = glm::mat4{ 1.0f };
		m_ProjectionMatrix[0][0] = 2.f / (Right - Left);
		m_ProjectionMatrix[1][1] = 2.f / (Bottom - Top);
		m_ProjectionMatrix[2][2] = 1.f / (Far - Near);
		m_ProjectionMatrix[3][0] = -(Right + Left) / (Right - Left);
		m_ProjectionMatrix[3][1] = -(Bottom + Top) / (Bottom - Top);
		m_ProjectionMatrix[3][2] = -Near / (Far - Near);
	}

	void Camera::SetPerspectiveProjection(float FovY, float Aspect, float Near, float Far)
	{
		assert(glm::abs(Aspect - std::numeric_limits<float>::epsilon()) > 0.0f);

		const float TanHalfFovy = tan(FovY / 2.f);
		m_ProjectionMatrix = glm::mat4{ 0.0f };
		m_ProjectionMatrix[0][0] = 1.f / (Aspect * TanHalfFovy);
		m_ProjectionMatrix[1][1] = 1.f / (TanHalfFovy);
		m_ProjectionMatrix[2][2] = Far / (Far - Near);
		m_ProjectionMatrix[2][3] = 1.f;
		m_ProjectionMatrix[3][2] = -(Far * Near) / (Far - Near);
	}

	void Camera::SetViewDirection(glm::vec3 Pos, glm::vec3 Dir, glm::vec3 Up)
	{
		const glm::vec3 w{ glm::normalize(Dir) };
		const glm::vec3 u{ glm::normalize(glm::cross(w, Up)) };
		const glm::vec3 v{ glm::cross(w, u) };

		m_ViewMatrix = glm::mat4{ 1.f };
		m_ViewMatrix[0][0] = u.x;
		m_ViewMatrix[1][0] = u.y;
		m_ViewMatrix[2][0] = u.z;
		m_ViewMatrix[0][1] = v.x;
		m_ViewMatrix[1][1] = v.y;
		m_ViewMatrix[2][1] = v.z;
		m_ViewMatrix[0][2] = w.x;
		m_ViewMatrix[1][2] = w.y;
		m_ViewMatrix[2][2] = w.z;
		m_ViewMatrix[3][0] = -glm::dot(u, Pos);
		m_ViewMatrix[3][1] = -glm::dot(v, Pos);
		m_ViewMatrix[3][2] = -glm::dot(w, Pos);
	}

	void Camera::SetViewTarget(glm::vec3 Pos, glm::vec3 Target, glm::vec3 Up)
	{
		SetViewDirection(Pos, Target - Pos, Up);
	}

	void Camera::SetViewYXZ(glm::vec3 Pos, glm::vec3 Rot)
	{
		const float c3 = glm::cos(Rot.z);
		const float s3 = glm::sin(Rot.z);
		const float c2 = glm::cos(Rot.x);
		const float s2 = glm::sin(Rot.x);
		const float c1 = glm::cos(Rot.y);
		const float s1 = glm::sin(Rot.y);
		const glm::vec3 u{ (c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1) };
		const glm::vec3 v{ (c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3) };
		const glm::vec3 w{ (c2 * s1), (-s2), (c1 * c2) };
		m_ViewMatrix = glm::mat4{ 1.f };
		m_ViewMatrix[0][0] = u.x;
		m_ViewMatrix[1][0] = u.y;
		m_ViewMatrix[2][0] = u.z;
		m_ViewMatrix[0][1] = v.x;
		m_ViewMatrix[1][1] = v.y;
		m_ViewMatrix[2][1] = v.z;
		m_ViewMatrix[0][2] = w.x;
		m_ViewMatrix[1][2] = w.y;
		m_ViewMatrix[2][2] = w.z;
		m_ViewMatrix[3][0] = -glm::dot(u, Pos);
		m_ViewMatrix[3][1] = -glm::dot(v, Pos);
		m_ViewMatrix[3][2] = -glm::dot(w, Pos);
	}
}