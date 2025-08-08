#ifndef __EngineMain_h__
#define __EngineMain_h__

#include "MyWindow.h"
#include "RenderPipeline.h"
#include "EngineDevice.h"
#include "EngineSwapChain.h"
#include "Mesh.h"
#include <memory>
#include <vector>
#include "GameObject.h"
#include <glm/gtc/constants.hpp>

namespace VulkanTutorial
{
	class EngineMain
	{
	public:

		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		EngineMain();
		virtual ~EngineMain();

		EngineMain(const EngineMain&) = delete;
		EngineMain& operator = (const EngineMain&) = delete;

		EngineMain(EngineMain&&) = delete;
		EngineMain& operator = (EngineMain&&) = delete;

		void Run();

	private:
		void LoadGameObjects();
		void CreatePipelineLayout();
		void CreatePipeline();
		void CreateCommandBuffers();
		void FreeCommandBuffers();
		void DrawFrame();
		void ReCreateSwapChain();
		void RecordCommandBuffer(int ImageIndex);
		void RenderGameObject(VkCommandBuffer CommandBuffer);

		MyWindow m_MyWindow = MyWindow("My Window", WIDTH, HEIGHT);
		EngineDevice m_EngineDevice = EngineDevice(m_MyWindow);
		std::unique_ptr<EngineSwapChain> m_SwapChain;
		std::unique_ptr<RenderPipeline> m_RenderPipeline;

		VkPipelineLayout m_PipelineLayout;
		std::vector<VkCommandBuffer> m_CommandBuffers;
		std::vector<GameObject> m_GameObjects;
	};
}

#endif //__EngineMain_h__
