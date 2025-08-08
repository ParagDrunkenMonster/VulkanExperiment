#include "RenderPipeline.h"
#include <fstream>
#include <iostream>
#include <cassert>
#include "Mesh.h"

namespace VulkanTutorial
{
	RenderPipeline::RenderPipeline(EngineDevice& Device, const PipelineConfigInfo& PipelineConfig, const std::string& VertProgram, const std::string& FragProgram)
		: m_EngineDevice(Device)
	{
		CreateGraphicsPipeline(PipelineConfig, VertProgram, FragProgram);
	}

	RenderPipeline::~RenderPipeline()
	{
		vkDestroyShaderModule(m_EngineDevice.Device(), m_VertexShaderModule, nullptr);
		vkDestroyShaderModule(m_EngineDevice.Device(), m_FragmentShaderModule, nullptr);
		vkDestroyPipeline(m_EngineDevice.Device(), m_VkGraphicsPipeline, nullptr);
	}

	void RenderPipeline::Bind(VkCommandBuffer CommandBuffer)
	{
		vkCmdBindPipeline(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_VkGraphicsPipeline);
	}

	void RenderPipeline::CreateGraphicsPipeline(const PipelineConfigInfo& PipelineConfig, const std::string& VertProgram, const std::string& FragProgram)
	{
		assert(PipelineConfig.PipelineLayout != VK_NULL_HANDLE && "Graphics pipeline can not be created:: no pipeline layout provided");
		assert(PipelineConfig.RenderPass != VK_NULL_HANDLE && "Graphics pipeline can not be created:: no render pass provided");

		std::vector<int8_t> VertexCode = ReadRile(VertProgram);
		std::vector<int8_t> FragmentCode = ReadRile(FragProgram);

		std::cout << "Vertex shader code size: " << VertexCode.size() << std::endl;
		std::cout << "Fragment shader code size: " << FragmentCode.size() << std::endl;

		CreateShaderModule(VertexCode, &m_VertexShaderModule);
		CreateShaderModule(FragmentCode, &m_FragmentShaderModule);

		VkPipelineShaderStageCreateInfo ShaderStates[2];
		ShaderStates[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		ShaderStates[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		ShaderStates[0].module = m_VertexShaderModule;
		ShaderStates[0].pName = "main";
		ShaderStates[0].flags = 0;
		ShaderStates[0].pNext = nullptr;
		ShaderStates[0].pSpecializationInfo = nullptr;

		ShaderStates[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		ShaderStates[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		ShaderStates[1].module = m_FragmentShaderModule;
		ShaderStates[1].pName = "main";
		ShaderStates[1].flags = 0;
		ShaderStates[1].pNext = nullptr;
		ShaderStates[1].pSpecializationInfo = nullptr;

		std::vector<VkVertexInputBindingDescription> BindingDescriptions = Mesh::Vertex::GetBindingDescriptions();
		std::vector<VkVertexInputAttributeDescription> AttributeDescriptions = Mesh::Vertex::GetAttributeDescriptions();

		VkPipelineVertexInputStateCreateInfo VertexInputInfo;
		VertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		VertexInputInfo.vertexAttributeDescriptionCount = (uint32_t)AttributeDescriptions.size();;
		VertexInputInfo.vertexBindingDescriptionCount = (uint32_t)BindingDescriptions.size();
		VertexInputInfo.pVertexAttributeDescriptions = AttributeDescriptions.data();
		VertexInputInfo.pVertexBindingDescriptions = BindingDescriptions.data();
		VertexInputInfo.flags = 0;
		VertexInputInfo.pNext = nullptr;

		VkGraphicsPipelineCreateInfo PipelineInfo{};
		PipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		PipelineInfo.flags = 0;
		PipelineInfo.stageCount = 2;
		PipelineInfo.pStages = ShaderStates;
		PipelineInfo.pVertexInputState = &VertexInputInfo;
		PipelineInfo.pInputAssemblyState = &PipelineConfig.InputAssemblyInfo;
		PipelineInfo.pViewportState = &PipelineConfig.ViewportInfo;
		PipelineInfo.pRasterizationState = &PipelineConfig.RasterizationInfo;
		PipelineInfo.pMultisampleState = &PipelineConfig.MultisampleInfo;
		PipelineInfo.pColorBlendState = &PipelineConfig.ColorBlendInfo;
		PipelineInfo.pDepthStencilState = &PipelineConfig.DepthStencilInfo;
		PipelineInfo.pDynamicState = &PipelineConfig.DynamicStateInfo;

		PipelineInfo.layout = PipelineConfig.PipelineLayout;
		PipelineInfo.renderPass = PipelineConfig.RenderPass;
		PipelineInfo.subpass = PipelineConfig.Subpass;

		PipelineInfo.basePipelineIndex = -1;
		PipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		PipelineInfo.pNext = nullptr;


		PipelineInfo.pTessellationState = nullptr;

		if (vkCreateGraphicsPipelines(m_EngineDevice.Device(), VK_NULL_HANDLE, 1, &PipelineInfo, nullptr, &m_VkGraphicsPipeline) != VK_SUCCESS)
			throw std::runtime_error("Failed to create graphics pipeline");
	}

	void RenderPipeline::CreateShaderModule(const std::vector<int8_t>& Code, VkShaderModule* ShaderModule)
	{
		VkShaderModuleCreateInfo CreateInfo{};
		CreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		CreateInfo.codeSize = Code.size();
		CreateInfo.pCode = (const uint32_t*)Code.data();

		if (vkCreateShaderModule(m_EngineDevice.Device(), &CreateInfo, nullptr, ShaderModule) != VK_SUCCESS)
			throw std::runtime_error("Failed to create shader module");
	}

	std::vector<int8_t> RenderPipeline::ReadRile(const std::string& FilePath)
	{
		std::ifstream File(FilePath, std::ios::ate | std::ios::binary);
		if (!File.is_open())
		{
			throw std::runtime_error("Failed to open file : " + FilePath);
		}

		size_t FileSize = static_cast<size_t>(File.tellg());
		std::vector<int8_t> Buffer(FileSize);

		File.seekg(0);

		File.read((char*)Buffer.data(), FileSize);

		return Buffer;
	}

	void RenderPipeline::DefaultPipelineConfigInfo(PipelineConfigInfo& ConfigInfo)
	{
		ConfigInfo.InputAssemblyInfo.flags = 0;
		ConfigInfo.InputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		ConfigInfo.InputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		ConfigInfo.InputAssemblyInfo.primitiveRestartEnable = VK_FALSE;
		ConfigInfo.InputAssemblyInfo.pNext = nullptr;

		ConfigInfo.ViewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		ConfigInfo.ViewportInfo.viewportCount = 1;
		ConfigInfo.ViewportInfo.pViewports = nullptr;
		ConfigInfo.ViewportInfo.scissorCount = 1;
		ConfigInfo.ViewportInfo.pScissors = nullptr;
		ConfigInfo.ViewportInfo.flags = 0;
		ConfigInfo.ViewportInfo.pNext = nullptr;

		ConfigInfo.RasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		ConfigInfo.RasterizationInfo.flags = 0;
		ConfigInfo.RasterizationInfo.depthClampEnable = VK_FALSE;
		ConfigInfo.RasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
		ConfigInfo.RasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
		ConfigInfo.RasterizationInfo.lineWidth = 1.0f;
		ConfigInfo.RasterizationInfo.cullMode = VK_CULL_MODE_NONE;
		ConfigInfo.RasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
		ConfigInfo.RasterizationInfo.depthBiasEnable = VK_FALSE;
		ConfigInfo.RasterizationInfo.depthBiasConstantFactor = 0.0f; //Optional
		ConfigInfo.RasterizationInfo.depthBiasClamp = 0.0f; //Optional
		ConfigInfo.RasterizationInfo.depthBiasSlopeFactor = 0.0f; //Optional
		ConfigInfo.RasterizationInfo.pNext = nullptr;

		ConfigInfo.MultisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		ConfigInfo.MultisampleInfo.flags = 0;
		ConfigInfo.MultisampleInfo.sampleShadingEnable = VK_FALSE;
		ConfigInfo.MultisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		ConfigInfo.MultisampleInfo.minSampleShading = 1.0f; //Optional
		ConfigInfo.MultisampleInfo.pSampleMask = nullptr; //Optional
		ConfigInfo.MultisampleInfo.alphaToCoverageEnable = VK_FALSE; //Optional
		ConfigInfo.MultisampleInfo.alphaToOneEnable = VK_FALSE; //Optional
		ConfigInfo.MultisampleInfo.pNext = nullptr;

		ConfigInfo.ColorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		ConfigInfo.ColorBlendAttachment.blendEnable = VK_FALSE;
		ConfigInfo.ColorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; //Optional
		ConfigInfo.ColorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; //Optional
		ConfigInfo.ColorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; //Optional;
		ConfigInfo.ColorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; //Optional
		ConfigInfo.ColorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; //Optional
		ConfigInfo.ColorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; //Optional
		
		ConfigInfo.ColorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		ConfigInfo.ColorBlendInfo.flags = 0;
		ConfigInfo.ColorBlendInfo.logicOpEnable = VK_FALSE;
		ConfigInfo.ColorBlendInfo.logicOp = VK_LOGIC_OP_COPY; //Optional
		ConfigInfo.ColorBlendInfo.attachmentCount = 1;
		ConfigInfo.ColorBlendInfo.pAttachments = &ConfigInfo.ColorBlendAttachment;
		ConfigInfo.ColorBlendInfo.blendConstants[0] = 0.0f;// Optional
		ConfigInfo.ColorBlendInfo.blendConstants[1] = 0.0f;// Optional
		ConfigInfo.ColorBlendInfo.blendConstants[2] = 0.0f;// Optional
		ConfigInfo.ColorBlendInfo.blendConstants[3] = 0.0f;// Optional
		ConfigInfo.ColorBlendInfo.pNext = nullptr;

		ConfigInfo.DepthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		ConfigInfo.DepthStencilInfo.flags = 0;
		ConfigInfo.DepthStencilInfo.depthTestEnable = VK_TRUE;
		ConfigInfo.DepthStencilInfo.depthWriteEnable = VK_TRUE;
		ConfigInfo.DepthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
		ConfigInfo.DepthStencilInfo.depthBoundsTestEnable = VK_FALSE;
		ConfigInfo.DepthStencilInfo.minDepthBounds = 0.0f; //Optional
		ConfigInfo.DepthStencilInfo.maxDepthBounds = 1.0f; //Optional
		ConfigInfo.DepthStencilInfo.stencilTestEnable = VK_FALSE;
		ConfigInfo.DepthStencilInfo.front = {}; //Optional
		ConfigInfo.DepthStencilInfo.back = {}; //Optional
		ConfigInfo.DepthStencilInfo.pNext = nullptr;

		ConfigInfo.DynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		ConfigInfo.DynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		ConfigInfo.DynamicStateInfo.pDynamicStates = ConfigInfo.DynamicStateEnables.data();
		ConfigInfo.DynamicStateInfo.dynamicStateCount = (uint32_t)ConfigInfo.DynamicStateEnables.size();
		ConfigInfo.DynamicStateInfo.flags = 0;
		ConfigInfo.DynamicStateInfo.pNext = nullptr;
	}
}