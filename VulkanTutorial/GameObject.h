#ifndef __GameObject_h__
#define __GameObject_h__

#include "Mesh.h"
#include <memory>

namespace VulkanTutorial
{
	struct Transform2dComponent
	{
		glm::vec2 Translation{0.0f, 0.0f};
		glm::vec2 Scale{ 1.0f, 1.0f };
		float Rotation = 0.0f;
		glm::mat2 Mat2() const 
		{	
			const float S = glm::sin(Rotation);
			const float C = glm::cos(Rotation);
			glm::mat2 RotMat{ {C, S}, {-S, C} };
			glm::mat2 ScaleMat{ {Scale.x, 0.0f}, {0.0f, Scale.y} };
			return RotMat * ScaleMat;
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

		void SetTransform2D(const Transform2dComponent& Transform) { m_Transform2D = Transform; }
		const Transform2dComponent& GetTransform2D() const { return m_Transform2D; }

		static GameObject CreateGameObject();

	private:

		GameObject(id_t Id);

		id_t m_Id;

		std::shared_ptr<Mesh> m_Mesh;
		glm::vec3 m_Color;
		Transform2dComponent m_Transform2D;
	};
}

#endif //__GameObject_h__
