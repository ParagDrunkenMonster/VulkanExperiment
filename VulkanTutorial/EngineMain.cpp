#include "EngineMain.h"
#include "BasicRenderSystem.h"

#include <stdexcept>
#include <array>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace VulkanTutorial
{
	EngineMain::EngineMain()
	{
		LoadGameObjects();
	}

	EngineMain::~EngineMain()
	{
		
	}

	void EngineMain::Run()
	{
		BasicRenderSystem SimpleRenderSystem(m_EngineDevice, m_Renderer.GetSwapChainRenderPass());

		while (m_MyWindow.IsOpen())
		{
			glfwPollEvents();
			
			if (auto CommandBuffer = m_Renderer.BeginFrame())
			{
				m_Renderer.BeginSwapChainRenderPass(CommandBuffer);
				SimpleRenderSystem.RenderGameObject(CommandBuffer, m_GameObjects);
				m_Renderer.EndSwapChainRenderPass(CommandBuffer);
				m_Renderer.EndFrame();
			}
		}

		vkDeviceWaitIdle(m_EngineDevice.Device());
	}

	void EngineMain::LoadGameObjects()
	{
		std::vector<Mesh::Vertex> Vertices{
			{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
			{{-0.0f, 0.5f}, {0.0f, 0.0f, 1.0f}}
		};

		std::shared_ptr<Mesh> NewMesh = std::make_shared<Mesh>(m_EngineDevice, Vertices);

		auto Triangle = GameObject::CreateGameObject();
		Triangle.SetMesh(NewMesh);
		Triangle.SetColor({0.2f, 0.8f, 0.1f});

		Transform2dComponent NewTransform;
		NewTransform.Translation.x = 0.2f;
		NewTransform.Scale = { 2.0f, 1.0f };
		NewTransform.Rotation = 0.25f * glm::two_pi<float>();

		Triangle.SetTransform2D(NewTransform);

		m_GameObjects.push_back(std::move(Triangle));
	}
}