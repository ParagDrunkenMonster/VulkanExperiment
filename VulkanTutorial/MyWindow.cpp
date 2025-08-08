#include "MyWindow.h"
#include <stdexcept>


namespace VulkanTutorial
{
	MyWindow::MyWindow(const std::string& WindowName, int Width, int Height)
		: m_GLTFWInitialized(false)
		, m_Window(nullptr)
		, m_WindowName(WindowName)
		, m_Width(Width)
		, m_Height(Height)
		, m_FrameBufferResized(false)
	{
		InitWindow();
	}

	MyWindow::~MyWindow()
	{
		DestroyWindow();
		glfwTerminate();
	}

	void MyWindow::InitWindow()
	{
		if (!m_GLTFWInitialized)
		{
			glfwInit();

			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		}

		if (m_Window == nullptr)
		{
			m_Window = glfwCreateWindow(m_Width, m_Height, m_WindowName.c_str(), nullptr, nullptr);
			glfwSetWindowUserPointer(m_Window, this);
			glfwSetFramebufferSizeCallback(m_Window, FrameBufferResizeCallback);
		}

		m_GLTFWInitialized = true;
	}

	void MyWindow::DestroyWindow()
	{
		if (m_Window)
		{
			glfwDestroyWindow(m_Window);
			m_Window = nullptr;
		}
	}

	void MyWindow::CreateWindowSurface(VkInstance VkInstance, VkSurfaceKHR* Surface)
	{
		if (glfwCreateWindowSurface(VkInstance, m_Window, nullptr, Surface) != VK_SUCCESS)
			throw std::runtime_error("Failed to create window surface");
	}

	bool MyWindow::IsOpen() const
	{
		return glfwWindowShouldClose(m_Window) == false;
	}

	void MyWindow::FrameBufferResizeCallback(GLFWwindow* GLWindow, int Width, int Height)
	{
		MyWindow* Window = reinterpret_cast<MyWindow*>(glfwGetWindowUserPointer(GLWindow));
		Window->m_Width = Width;
		Window->m_Height = Height;
		Window->m_FrameBufferResized = true;
	}
}