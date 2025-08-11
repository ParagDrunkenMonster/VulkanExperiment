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
		glm::mat4 Mat4() const;
		glm::mat3 NormalMatrix() const;
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
