#include "BasicRenderSystem.h"

#include <stdexcept>
#include <array>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace VulkanTutorial
{
	struct SimplePushConstantData
	{
		glm::mat4 transform = glm::mat2(1.0f);
		alignas (16) glm::vec3 color;
	};

	BasicRenderSystem::BasicRenderSystem(EngineDevice& Device, VkRenderPass RenderPass)
		: m_EngineDevice(Device)
	{
		CreatePipelineLayout();
		CreatePipeline(RenderPass);
	}

	BasicRenderSystem::~BasicRenderSystem()
	{
		vkDestroyPipelineLayout(m_EngineDevice.Device(), m_PipelineLayout, nullptr);
	}

	void BasicRenderSystem::CreatePipelineLayout()
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

	void BasicRenderSystem::CreatePipeline(VkRenderPass RenderPass)
	{
		assert(m_PipelineLayout != nullptr && "Can not create pipeline before pipeline layout");

		PipelineConfigInfo PipelineConfig;
		RenderPipeline::DefaultPipelineConfigInfo(PipelineConfig);
		PipelineConfig.RenderPass = RenderPass;
		PipelineConfig.PipelineLayout = m_PipelineLayout;

		// If render pass compatible do nothing else
		m_RenderPipeline = std::make_unique<RenderPipeline>(m_EngineDevice, PipelineConfig, "./../../Content/VertexShader.vert.spv", "./../../Content/PixelShader.frag.spv");
	}

	void BasicRenderSystem::RenderGameObject(VkCommandBuffer CommandBuffer, std::vector<GameObject>& GameObjects, const Camera& Cam)
	{
		m_RenderPipeline->Bind(CommandBuffer);

		auto ProjectionView = Cam.GetProjectionMatrix() * Cam.GetViewMatrix();

		for (auto& Obj : GameObjects)
		{
			SimplePushConstantData Push;

			Push.color = Obj.GetColor();
			Push.transform = ProjectionView * Obj.GetTransform().Mat4();

			vkCmdPushConstants(CommandBuffer, m_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
				, 0, sizeof(SimplePushConstantData), &Push);

			Obj.GetMesh()->Bind(CommandBuffer);
			Obj.GetMesh()->Draw(CommandBuffer);
		}
	}
}