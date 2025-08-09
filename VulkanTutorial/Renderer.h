#ifndef __Renderer_h__
#define __Renderer_h__

#include "EngineDevice.h"
#include "EngineSwapChain.h"
#include "MyWindow.h"

#include <memory>
#include <vector>
#include <cassert>

namespace VulkanTutorial
{
	class Renderer
	{
	public:

		Renderer(MyWindow& MyWindow, EngineDevice& EngineDevice);
		virtual ~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer& operator = (const Renderer&) = delete;

		Renderer(Renderer&&) = delete;
		Renderer& operator = (Renderer&&) = delete;

		VkRenderPass GetSwapChainRenderPass() const { return m_SwapChain->GetRenderPass(); }
		bool IsFrameInProgress() const { return m_IsFrameStarted; }

		VkCommandBuffer GetCommandBuffer() const 
		{
			assert(m_IsFrameStarted && "Can not get command buffer when frame is not in progress");
			return m_CommandBuffers[m_CurrentImageIndex]; 
		}

		VkCommandBuffer BeginFrame();
		void EndFrame();

		void BeginSwapChainRenderPass(VkCommandBuffer CommandBuffer);
		void EndSwapChainRenderPass(VkCommandBuffer CommandBuffer);

	private:
		void CreateCommandBuffers();
		void FreeCommandBuffers();
		void ReCreateSwapChain();

		MyWindow& m_MyWindow;
		EngineDevice& m_EngineDevice;
		std::unique_ptr<EngineSwapChain> m_SwapChain;
		std::vector<VkCommandBuffer> m_CommandBuffers;

		uint32_t m_CurrentImageIndex;
		bool m_IsFrameStarted;
	};
}

#endif //__Renderer_h__
