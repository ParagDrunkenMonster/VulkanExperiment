#include "EngineMain.h"
#include "BasicRenderSystem.h"

#include <stdexcept>
#include <array>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include "KeyboardController.h"

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
		Camera Cam;
		Cam.SetViewDirection(glm::vec3(0.0f), glm::vec3(0.5f, 0.0f, 1.0f));

		auto ViewerObject = GameObject::CreateGameObject();
		KeyboardController CameraController;

		auto CurrentTime = std::chrono::high_resolution_clock::now();

		while (m_MyWindow.IsOpen())
		{
			glfwPollEvents();

			auto NewTime = std::chrono::high_resolution_clock::now();
			float FrameTime = std::chrono::duration<float, std::chrono::seconds::period>(NewTime - CurrentTime).count();
			CurrentTime = NewTime;

			FrameTime = glm::min(FrameTime, MAX_FRAME_TIME);

			CameraController.MoveInPaneXZ(m_MyWindow.GetGLFWwindow(), FrameTime, ViewerObject);
			Cam.SetViewYXZ(ViewerObject.GetTransform().Translation, ViewerObject.GetTransform().Rotation);

			const float Aspect = m_Renderer.GetAspectRatio();
			//Cam.SetOrthographicsProjection(-Aspect, Aspect, -1, 1, -1, 1);
			Cam.SetPerspectiveProjection(glm::radians(50.0f), Aspect, 0.1f, 10.0f);

			if (auto CommandBuffer = m_Renderer.BeginFrame())
			{
				m_Renderer.BeginSwapChainRenderPass(CommandBuffer);
				SimpleRenderSystem.RenderGameObject(CommandBuffer, m_GameObjects, Cam);
				m_Renderer.EndSwapChainRenderPass(CommandBuffer);
				m_Renderer.EndFrame();
			}
		}

		vkDeviceWaitIdle(m_EngineDevice.Device());
	}

	std::unique_ptr<Mesh> CreateCubeModel(EngineDevice& Device, glm::vec3 Offset) {
		Mesh::Builder ModelBuilder;
		
		ModelBuilder.Vertices = {
			// left face (white)
			{{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
			{{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},

			// right face (yellow)
			{{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .8f, .1f}},

			// top face (orange, remember y axis points down)
			{{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
			{{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},

			// bottom face (red)
			{{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
			{{.5f, .5f, -.5f}, {.8f, .1f, .1f}},

			// nose face (blue)
			{{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
			{{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},

			// tail face (green)
			{{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
			{{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
		};
		for (auto& v : ModelBuilder.Vertices) {
			v.position += Offset;
		}

		ModelBuilder.Indices = { 0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
								12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21 };

		return std::make_unique<Mesh>(Device, ModelBuilder);
	}

	void EngineMain::LoadGameObjects()
	{
		/*std::shared_ptr<Mesh> NewMesh = CreateCubeModel(m_EngineDevice, {0.0f, 0.0f, 0.0f});
		auto Cube = GameObject::CreateGameObject();
		Cube.SetMesh(NewMesh);

		TransformComponent Transform;
		Transform.Translation = { 0.0f, 0.0f, 0.5f };
		Transform.Scale = { 0.25f, 0.25f, 0.25f };

		Cube.SetTransform(Transform);

		m_GameObjects.push_back(std::move(Cube));
		*/

		std::shared_ptr<Mesh> NewMesh = Mesh::CreateModelFromFile(m_EngineDevice, "./../../Content/smooth_vase.obj");
		auto Cube = GameObject::CreateGameObject();
		Cube.SetMesh(NewMesh);

		TransformComponent Transform;
		Transform.Translation = { 0.0f, 0.0f, 0.5f };
		Transform.Scale = { 0.25f, 0.25f, 0.25f };

		Cube.SetTransform(Transform);

		m_GameObjects.push_back(std::move(Cube));
	}
}