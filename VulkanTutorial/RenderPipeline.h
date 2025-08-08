#ifndef __RenderPipeline_h__
#define __RenderPipeline_h__

#include <string>
#include <vector>
#include "EngineDevice.h"

namespace  VulkanTutorial
{
	struct PipelineConfigInfo
	{
		PipelineConfigInfo() = default;
		PipelineConfigInfo(const PipelineConfigInfo&) = delete;
		PipelineConfigInfo& operator = (const PipelineConfigInfo&) = delete;

		VkPipelineViewportStateCreateInfo ViewportInfo;
		VkPipelineInputAssemblyStateCreateInfo InputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo RasterizationInfo;
		VkPipelineMultisampleStateCreateInfo MultisampleInfo;
		VkPipelineColorBlendAttachmentState ColorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo ColorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo DepthStencilInfo;
		std::vector<VkDynamicState> DynamicStateEnables;
		VkPipelineDynamicStateCreateInfo DynamicStateInfo;
		VkPipelineLayout PipelineLayout = nullptr;
		VkRenderPass RenderPass = nullptr;
		uint32_t Subpass = 0;
	};

	class RenderPipeline
	{
	public:

		RenderPipeline(EngineDevice& Device, const PipelineConfigInfo& PipelineConfig, const std::string& VertProgram, const std::string& FragProgram);
		virtual ~RenderPipeline();

		RenderPipeline(const RenderPipeline&) = delete;
		RenderPipeline& operator = (const RenderPipeline&) = delete;

		RenderPipeline(RenderPipeline&&) = delete;
		RenderPipeline& operator = (RenderPipeline&&) = delete;

		void Bind(VkCommandBuffer CommandBuffer);

		static void DefaultPipelineConfigInfo(PipelineConfigInfo& ConfigInfo);

	private:

		static std::vector<int8_t> ReadRile(const std::string& FilePath);

		void CreateGraphicsPipeline(const PipelineConfigInfo& PipelineConfig, const std::string& VertProgram, const std::string& FragProgram);

		void CreateShaderModule(const std::vector<int8_t>& Code, VkShaderModule* ShaderModule);

		EngineDevice& m_EngineDevice;
		VkPipeline m_VkGraphicsPipeline;
		VkShaderModule m_VertexShaderModule;
		VkShaderModule m_FragmentShaderModule;
	};
}

#endif //__RenderPipeline_h__
