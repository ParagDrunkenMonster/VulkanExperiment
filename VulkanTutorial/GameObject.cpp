#include "GameObject.h"

namespace VulkanTutorial
{
	GameObject::GameObject(id_t Id)
		: m_Id(Id)
	{

	}

	GameObject GameObject::CreateGameObject()
	{
		static id_t CurrentId = 0;
		return GameObject(CurrentId++);
	}
}