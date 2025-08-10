#ifndef __BasicRenderSystem_h__
#define __BasicRenderSystem_h__

#include "RenderPipeline.h"
#include "EngineDevice.h"
#include "GameObject.h"
#include "Camera.h"

#include <memory>
#include <vector>


namespace VulkanTutorial
{
	class BasicRenderSystem
	{
	public:

		BasicRenderSystem(EngineDevice& Device, VkRenderPass RenderPass);
		virtual ~BasicRenderSystem();

		BasicRenderSystem(const BasicRenderSystem&) = delete;
		BasicRenderSystem& operator = (const BasicRenderSystem&) = delete;

		BasicRenderSystem(BasicRenderSystem&&) = delete;
		BasicRenderSystem& operator = (BasicRenderSystem&&) = delete;

		void RenderGameObject(VkCommandBuffer CommandBuffer, std::vector<GameObject>& GameObjects, const Camera& Cam);

	private:

		void CreatePipelineLayout();
		void CreatePipeline(VkRenderPass RenderPass);

		EngineDevice& m_EngineDevice;

		std::unique_ptr<RenderPipeline> m_RenderPipeline;

		VkPipelineLayout m_PipelineLayout;
	};
}

#endif //__BasicRenderSystem_h__