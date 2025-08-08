#include "EngineMain.h"

#include <stdexcept>
#include <array>

#include <glm/glm.hpp>

namespace VulkanTutorial
{
	struct SimplePushConstantData
	{
		glm::mat2 transform = glm::mat2(1.0f);
		glm::vec2 offset;
		alignas (16) glm::vec3 color;
	};

	EngineMain::EngineMain()
	{
		LoadGameObjects();
		CreatePipelineLayout();
		ReCreateSwapChain();
		CreateCommandBuffers();
	}

	EngineMain::~EngineMain()
	{
		vkDestroyPipelineLayout(m_EngineDevice.Device(), m_PipelineLayout, nullptr);
	}

	void EngineMain::Run()
	{
		while (m_MyWindow.IsOpen())
		{
			glfwPollEvents();
			DrawFrame();
		}

		vkDeviceWaitIdle(m_EngineDevice.Device());
	}

	void EngineMain::CreatePipelineLayout()
	{
		VkPushConstantRange PushConstantRange;
		PushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		PushConstantRange.offset = 0;
		PushConstantRange.size = sizeof(SimplePushConstantData);

		VkPipelineLayoutCreateInfo PipelineLayoutInfo;
		PipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		PipelineLayoutInfo.setLayoutCount = 0;
		PipelineLayoutInfo.pSetLayouts = nullptr;
		PipelineLayoutInfo.pushConstantRangeCount = 1;
		PipelineLayoutInfo.pPushConstantRanges = &PushConstantRange;
		PipelineLayoutInfo.pNext = nullptr;
		PipelineLayoutInfo.flags = 0;
		if (vkCreatePipelineLayout(m_EngineDevice.Device(), &PipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
			throw std::runtime_error("Failed to create pipeline layout!");
	}

	void EngineMain::CreatePipeline()
	{
		PipelineConfigInfo PipelineConfig;
		RenderPipeline::DefaultPipelineConfigInfo(PipelineConfig);
		PipelineConfig.RenderPass = m_SwapChain->GetRenderPass();
		PipelineConfig.PipelineLayout = m_PipelineLayout;

		// If render pass compatible do nothing else
		m_RenderPipeline = std::make_unique<RenderPipeline>(m_EngineDevice, PipelineConfig, "./../../Content/VertexShader.vert.spv", "./../../Content/PixelShader.frag.spv");
	}

	void EngineMain::ReCreateSwapChain()
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
			m_SwapChain = std::make_unique<EngineSwapChain>(m_EngineDevice, Extent, std::move(m_SwapChain));
			if (m_SwapChain->ImageCount() != m_CommandBuffers.size())
			{
				FreeCommandBuffers();
				CreateCommandBuffers();
			}
		}
		CreatePipeline();
	}

	void EngineMain::CreateCommandBuffers()
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

	void EngineMain::RecordCommandBuffer(int ImageIndex)
	{
		VkCommandBufferBeginInfo BeginInfo;
		BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;;
		BeginInfo.pNext = nullptr;
		BeginInfo.flags = 0;

		if (vkBeginCommandBuffer(m_CommandBuffers[ImageIndex], &BeginInfo) != VK_SUCCESS)
			throw std::runtime_error("Failed to allocate command buffer begin info");

		VkRenderPassBeginInfo RenderPassInfo;
		RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		RenderPassInfo.renderPass = m_SwapChain->GetRenderPass();
		RenderPassInfo.framebuffer = m_SwapChain->GetFrameBuffer(ImageIndex);

		RenderPassInfo.renderArea.offset = { 0, 0 };
		RenderPassInfo.renderArea.extent = m_SwapChain->GetSwapChainExtent();

		std::array<VkClearValue, 2> ClearValues;

		ClearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
		ClearValues[1].depthStencil = { 1.0f, 0 };

		RenderPassInfo.clearValueCount = (uint32_t)ClearValues.size();
		RenderPassInfo.pClearValues = ClearValues.data();
		RenderPassInfo.pNext = nullptr;

		vkCmdBeginRenderPass(m_CommandBuffers[ImageIndex], &RenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport Viewport;
		Viewport.x = 0.0f;
		Viewport.y = 0.0f;
		Viewport.width = (float)m_SwapChain->GetSwapChainExtent().width;
		Viewport.height = (float)m_SwapChain->GetSwapChainExtent().height;
		Viewport.minDepth = 0.0f;
		Viewport.maxDepth = 1.0f;
		
		VkRect2D Scisor{ {0, 0}, m_SwapChain->GetSwapChainExtent()};
		vkCmdSetViewport(m_CommandBuffers[ImageIndex], 0, 1, &Viewport);
		vkCmdSetScissor(m_CommandBuffers[ImageIndex], 0, 1, &Scisor);

		RenderGameObject(m_CommandBuffers[ImageIndex]);

		vkCmdEndRenderPass(m_CommandBuffers[ImageIndex]);
		if (vkEndCommandBuffer(m_CommandBuffers[ImageIndex]) != VK_SUCCESS)
			throw std::runtime_error("Failed to record command");
	}

	void EngineMain::FreeCommandBuffers()
	{
		vkFreeCommandBuffers(m_EngineDevice.Device(), m_EngineDevice.GetCommandPool(), (uint32_t)m_CommandBuffers.size(), m_CommandBuffers.data());
		m_CommandBuffers.clear();
	}

	void EngineMain::RenderGameObject(VkCommandBuffer CommandBuffer)
	{
		m_RenderPipeline->Bind(CommandBuffer);

		for (auto& Obj : m_GameObjects)
		{
			SimplePushConstantData Push;
			//float Rotation = Obj.GetTransform2D().Rotation;
			//Rotation = glm::mod(Rotation + 0.01f, glm::two_pi<float>());
			Push.offset = Obj.GetTransform2D().Translation;
			Push.color = Obj.GetColor();
			Push.transform = Obj.GetTransform2D().Mat2();

			vkCmdPushConstants(CommandBuffer, m_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
				, 0, sizeof(SimplePushConstantData), &Push);

			Obj.GetMesh()->Bind(CommandBuffer);
			Obj.GetMesh()->Draw(CommandBuffer);
		}
	}

	void EngineMain::DrawFrame()
	{
		uint32_t ImageIndex;
		auto result = m_SwapChain->AcquireNextImage(&ImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			ReCreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
			throw std::runtime_error("Failed to acquire swap chain image!");

		RecordCommandBuffer(ImageIndex);
		result = m_SwapChain->SubmitCommandBuffers(&m_CommandBuffers[ImageIndex], &ImageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_MyWindow.WasWindowResized())
		{
			m_MyWindow.ResetWindowResizedFlag();
			ReCreateSwapChain();
			return;
		}

		if (result != VK_SUCCESS)
			throw std::runtime_error("Failed to present swap chain image!");
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