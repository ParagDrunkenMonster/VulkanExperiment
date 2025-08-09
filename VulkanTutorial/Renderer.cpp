#include "Renderer.h"
#include <stdexcept>
#include <array>

namespace VulkanTutorial
{
	Renderer::Renderer(MyWindow& MyWindow, EngineDevice& EngineDevice)
		: m_MyWindow(MyWindow)
		, m_EngineDevice(EngineDevice)
	{
		ReCreateSwapChain();
		CreateCommandBuffers();
	}

	Renderer::~Renderer()
	{
		FreeCommandBuffers();
	}

	void Renderer::ReCreateSwapChain()
	{
		VkExtent2D Extent = m_MyWindow.GetExtent();
		while (Extent.width == 0 || Extent.height == 0)
		{
			Extent = m_MyWindow.GetExtent();
			glfwWaitEvents();
		}

		vkDeviceWaitIdle(m_EngineDevice.Device());

		if (m_SwapChain == nullptr)
		{
			m_SwapChain = std::make_unique<EngineSwapChain>(m_EngineDevice, Extent);
		}
		else
		{
			std::shared_ptr<EngineSwapChain> OldSwapChain = std::move(m_SwapChain);
			m_SwapChain = std::make_unique<EngineSwapChain>(m_EngineDevice, Extent, OldSwapChain);

			if (!OldSwapChain->CompareSwapFormats(*m_SwapChain.get()))
			{
				throw std::runtime_error("Swap chains are not compatible");
			}

			if (m_SwapChain->ImageCount() != m_CommandBuffers.size())
			{
				FreeCommandBuffers();
				CreateCommandBuffers();
			}
		}
	}

	void Renderer::CreateCommandBuffers()
	{
		m_CommandBuffers.resize(m_SwapChain->ImageCount());

		VkCommandBufferAllocateInfo AllocInfo;
		AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		AllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		AllocInfo.commandPool = m_EngineDevice.GetCommandPool();
		AllocInfo.commandBufferCount = (uint32_t)m_CommandBuffers.size();
		AllocInfo.pNext = nullptr;

		if (vkAllocateCommandBuffers(m_EngineDevice.Device(), &AllocInfo, m_CommandBuffers.data()) != VK_SUCCESS)
			throw std::runtime_error("Failed to allocate command buffers");
	}

	void Renderer::FreeCommandBuffers()
	{
		vkFreeCommandBuffers(m_EngineDevice.Device(), m_EngineDevice.GetCommandPool(), (uint32_t)m_CommandBuffers.size(), m_CommandBuffers.data());
		m_CommandBuffers.clear();
	}

	VkCommandBuffer Renderer::BeginFrame()
	{
		assert(!m_IsFrameStarted && "Can not call begin frame while already in progress");

		auto result = m_SwapChain->AcquireNextImage(&m_CurrentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			ReCreateSwapChain();
			return nullptr;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
			throw std::runtime_error("Failed to acquire swap chain image!");

		m_IsFrameStarted = true;
		auto CommandBuffer = GetCommandBuffer();

		VkCommandBufferBeginInfo BeginInfo;
		BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;;
		BeginInfo.pNext = nullptr;
		BeginInfo.flags = 0;

		if (vkBeginCommandBuffer(CommandBuffer, &BeginInfo) != VK_SUCCESS)
			throw std::runtime_error("Failed to allocate command buffer begin info");

		return CommandBuffer;
	}

	void Renderer::EndFrame()
	{
		assert(m_IsFrameStarted && "Can not call endframe while frame is not in progress");
		
		auto CommandBuffer = GetCommandBuffer();

		if (vkEndCommandBuffer(CommandBuffer) != VK_SUCCESS)
			throw std::runtime_error("Failed to record command");

		auto result = m_SwapChain->SubmitCommandBuffers(&CommandBuffer, &m_CurrentImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_MyWindow.WasWindowResized())
		{
			m_MyWindow.ResetWindowResizedFlag();
			ReCreateSwapChain();
		}
		else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to present swap chain image!");
		}

		m_IsFrameStarted = false;
	}

	void Renderer::BeginSwapChainRenderPass(VkCommandBuffer CommandBuffer)
	{
		assert(m_IsFrameStarted && "Can not call BeginSwapChainRenderPass if frame is not in progress");
		assert(CommandBuffer == GetCommandBuffer() && "Can not begin render pass on command buffer from a different frame");

		VkRenderPassBeginInfo RenderPassInfo;
		RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		RenderPassInfo.renderPass = m_SwapChain->GetRenderPass();
		RenderPassInfo.framebuffer = m_SwapChain->GetFrameBuffer(m_CurrentImageIndex);

		RenderPassInfo.renderArea.offset = { 0, 0 };
		RenderPassInfo.renderArea.extent = m_SwapChain->GetSwapChainExtent();

		std::array<VkClearValue, 2> ClearValues;

		ClearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
		ClearValues[1].depthStencil = { 1.0f, 0 };

		RenderPassInfo.clearValueCount = (uint32_t)ClearValues.size();
		RenderPassInfo.pClearValues = ClearValues.data();
		RenderPassInfo.pNext = nullptr;

		vkCmdBeginRenderPass(CommandBuffer, &RenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport Viewport;
		Viewport.x = 0.0f;
		Viewport.y = 0.0f;
		Viewport.width = (float)m_SwapChain->GetSwapChainExtent().width;
		Viewport.height = (float)m_SwapChain->GetSwapChainExtent().height;
		Viewport.minDepth = 0.0f;
		Viewport.maxDepth = 1.0f;

		VkRect2D Scisor{ {0, 0}, m_SwapChain->GetSwapChainExtent() };
		vkCmdSetViewport(CommandBuffer, 0, 1, &Viewport);
		vkCmdSetScissor(CommandBuffer, 0, 1, &Scisor);
	}

	void Renderer::EndSwapChainRenderPass(VkCommandBuffer CommandBuffer)
	{
		assert(m_IsFrameStarted && "Can not call EndSwapChainRenderPass if frame is not in progress");
		assert(CommandBuffer == GetCommandBuffer() && "Can not end render pass on command buffer from a different frame");

		vkCmdEndRenderPass(CommandBuffer);
	}
}