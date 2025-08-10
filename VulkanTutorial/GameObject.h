#ifndef __GameObject_h__
#define __GameObject_h__

#include "Mesh.h"
#include <memory>

#include <glm/gtc/matrix_transform.hpp>

namespace VulkanTutorial
{
	struct TransformComponent
	{
		glm::vec3 Translation{ 0.0f, 0.0f, 0.0f};
		glm::vec3 Scale{ 1.0f, 1.0f, 1.0f};
		glm::vec3 Rotation{ 0.0f, 0.0f, 0.0f };
		glm::mat4 Mat4() const 
		{	
			const float c3 = glm::cos(Rotation.z);
			const float s3 = glm::sin(Rotation.z);
			const float c2 = glm::cos(Rotation.x);
			const float s2 = glm::sin(Rotation.x);
			const float c1 = glm::cos(Rotation.y);
			const float s1 = glm::sin(Rotation.y);
			return glm::mat4{
				{
					Scale.x * (c1 * c3 + s1 * s2 * s3),
					Scale.x * (c2 * s3),
					Scale.x * (c1 * s2 * s3 - c3 * s1),
					0.0f,
				},
				{
					Scale.y * (c3 * s1 * s2 - c1 * s3),
					Scale.y * (c2 * c3),
					Scale.y * (c1 * c3 * s2 + s1 * s3),
					0.0f,
				},
				{
					Scale.z * (c2 * s1),
					Scale.z * (-s2),
					Scale.z * (c1 * c2),
					0.0f,
				},
				{Translation.x, Translation.y, Translation.z, 1.0f} };
		}
	};

	class GameObject
	{
	public:

		using id_t = unsigned int;

		GameObject(const GameObject&) = delete;
		GameObject& operator = (const GameObject&) = delete;
		GameObject(GameObject&&) = default;
		GameObject& operator = (GameObject&&) = default;

		id_t GetId() const { return m_Id; }

		void SetMesh(std::shared_ptr<Mesh> Mesh) { m_Mesh = Mesh; }
		std::shared_ptr<Mesh> GetMesh() { return m_Mesh; }

		void SetColor(const glm::vec3& Color) { m_Color = Color; }
		const glm::vec3& GetColor() const { return m_Color; }

		void SetTransform(const TransformComponent& Transform) { m_Transform = Transform; }
		const TransformComponent& GetTransform() const { return m_Transform; }

		static GameObject CreateGameObject();

	private:

		GameObject(id_t Id);

		id_t m_Id;

		std::shared_ptr<Mesh> m_Mesh;
		glm::vec3 m_Color;
		TransformComponent m_Transform;
	};
}

#endif //__GameObject_h__
