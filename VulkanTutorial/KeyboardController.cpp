#include "KeyboardController.h"

namespace VulkanTutorial
{
	KeyboardController::KeyboardController()
	{

	}

	KeyboardController::~KeyboardController()
	{

	}

	void KeyboardController::MoveInPaneXZ(GLFWwindow* Window, float Dt, GameObject& GO)
	{
		glm::vec3 Rotate{ 0 };
		if (glfwGetKey(Window, Keys.LookRight) == GLFW_PRESS) Rotate.y += 1.0f;
		if (glfwGetKey(Window, Keys.LookLeft) == GLFW_PRESS) Rotate.y -= 1.0f;
		if (glfwGetKey(Window, Keys.LookUp) == GLFW_PRESS) Rotate.x += 1.0f;
		if (glfwGetKey(Window, Keys.LookDown) == GLFW_PRESS) Rotate.x -= 1.0f;

		TransformComponent Transform = GO.GetTransform();
		if (glm::dot(Rotate, Rotate) > std::numeric_limits<float>::epsilon())
		{
			Transform.Rotation += LookSpeed * Dt * glm::normalize(Rotate);
		}

		Transform.Rotation.x = glm::clamp(Transform.Rotation.x, -1.5f, 1.5f);
		Transform.Rotation.y = glm::mod(Transform.Rotation.y, glm::two_pi<float>());

		GO.SetTransform(Transform);
	}
}