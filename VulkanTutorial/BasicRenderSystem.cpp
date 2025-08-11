#include "BasicRenderSystem.h"

#include <stdexcept>
#include <array>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace VulkanTutorial
{
	struct SimplePushConstantData
	{
		glm::mat4 modelMatrix = glm::mat2(1.0f);
		glm::mat4 normalMatrix = glm::mat2(1.0f);
	};

	BasicRenderSystem::BasicRenderSystem(EngineDevice& Device, VkRenderPass RenderPass, VkDescriptorSetLayout GlobalSetLayout)
		: m_EngineDevice(Device)
	{
		CreatePipelineLayout(GlobalSetLayout);
		CreatePipeline(RenderPass);
	}

	BasicRenderSystem::~BasicRenderSystem()
	{
		vkDestroyPipelineLayout(m_EngineDevice.Device(), m_PipelineLayout, nullptr);
	}

	void BasicRenderSystem::CreatePipelineLayout(VkDescriptorSetLayout GlobalSetLayout)
	{
		VkPushConstantRange PushConstantRange;
		PushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		PushConstantRange.offset = 0;
		PushConstantRange.size = sizeof(SimplePushConstantData);

		std::vector<VkDescriptorSetLayout> DescriptorSetLayouts{ GlobalSetLayout };

		VkPipelineLayoutCreateInfo PipelineLayoutInfo;
		PipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		PipelineLayoutInfo.setLayoutCount = (uint32_t)DescriptorSetLayouts.size();
		PipelineLayoutInfo.pSetLayouts = DescriptorSetLayouts.data();
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

	void BasicRenderSystem::RenderGameObject(FrameInfo& Info, std::vector<GameObject>& GameObjects)
	{
		m_RenderPipeline->Bind(Info.CommandBuffer);

		vkCmdBindDescriptorSets(Info.CommandBuffer
			, VK_PIPELINE_BIND_POINT_GRAPHICS
			, m_PipelineLayout
			, 0, 1
			, &Info.GlobalDescriptorSet
			, 0
			, nullptr);

		for (auto& Obj : GameObjects)
		{
			SimplePushConstantData Push;

			auto ModelMatrix = Obj.GetTransform().Mat4();

			Push.normalMatrix = Obj.GetTransform().NormalMatrix();
			Push.modelMatrix = ModelMatrix;

			vkCmdPushConstants(Info.CommandBuffer, m_PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT
				, 0, sizeof(SimplePushConstantData), &Push);

			Obj.GetMesh()->Bind(Info.CommandBuffer);
			Obj.GetMesh()->Draw(Info.CommandBuffer);
		}
	}
}