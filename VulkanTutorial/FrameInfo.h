#ifndef __FrameInfo_h__
#define __FrameInfo_h__

#include "Camera.h"

#include <vulkan/vulkan.h>

namespace VulkanTutorial
{
	struct FrameInfo
	{
		const int FrameIndex;
		const float FrameTime;
		VkCommandBuffer CommandBuffer;
		Camera& Cam;
		VkDescriptorSet GlobalDescriptorSet;
	};
}


#endif //__FrameInfo_h__
