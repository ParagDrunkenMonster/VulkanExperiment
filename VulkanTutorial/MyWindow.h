#ifndef __MyWindow_h__
#define __MyWindow_h__
#include <GLFW/glfw3.h>
#include <string>

namespace VulkanTutorial
{
	class MyWindow
	{
	public:
	
		MyWindow(const std::string& WindowName, int Width, int Height);
		virtual ~MyWindow();

		MyWindow(const MyWindow&) = delete;
		MyWindow& operator = (const MyWindow&) = delete;

		MyWindow(MyWindow&&) = delete;
		MyWindow& operator = (MyWindow&&) = delete;

		bool IsOpen() const;
		VkExtent2D GetExtent() const { return { (uint32_t)m_Width, (uint32_t)m_Height }; }
		bool WasWindowResized() const { return m_FrameBufferResized; }
		void ResetWindowResizedFlag() { m_FrameBufferResized = false; }
		GLFWwindow* GetGLFWwindow() const { return m_Window; }

		void CreateWindowSurface(VkInstance VkInstance, VkSurfaceKHR* Surface);
	private:
		
		static void FrameBufferResizeCallback(GLFWwindow* GLWindow, int Width, int Height);

		void InitWindow();
		void DestroyWindow();

		bool m_GLTFWInitialized;

		GLFWwindow* m_Window;

		const std::string m_WindowName;

		int m_Width;
		int m_Height;
		bool m_FrameBufferResized;
	};
}

#endif //__MyWindow_h__