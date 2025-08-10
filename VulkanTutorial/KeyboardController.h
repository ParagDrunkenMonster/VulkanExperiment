#ifndef __KeyboardController_h__
#define __KeyboardController_h__

#include "GameObject.h"
#include "MyWindow.h"

namespace VulkanTutorial
{
	class KeyboardController
	{
	public:

		struct KeyMappings
		{
			int MoveLeft = GLFW_KEY_A;
			int MoveRight = GLFW_KEY_D;
			int MoveForward = GLFW_KEY_W;
			int MoveBackward = GLFW_KEY_S;
			int MoveUp = GLFW_KEY_E;
			int MoveDown = GLFW_KEY_Q;
			int LookLeft = GLFW_KEY_LEFT;
			int LookRight = GLFW_KEY_RIGHT;
			int LookUp = GLFW_KEY_UP;
			int LookDown = GLFW_KEY_DOWN;
		};

		KeyMappings Keys{};
		float MoveSpeed = 3.0f;
		float LookSpeed = 1.5f;

		KeyboardController();
		virtual ~KeyboardController();

		KeyboardController(const KeyboardController&) = delete;
		KeyboardController& operator = (const KeyboardController&) = delete;

		KeyboardController(KeyboardController&&) = delete;
		KeyboardController& operator = (KeyboardController&&) = delete;

		void MoveInPaneXZ(GLFWwindow* Window, float Dt, GameObject& GO);

	private:
	};
}

#endif
