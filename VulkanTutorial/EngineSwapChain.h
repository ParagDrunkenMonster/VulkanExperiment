#ifndef __EngineSwapChain_h__
#define __EngineSwapChain_h__

#include "EngineDevice.h"

// vulkan headers
#include <vulkan/vulkan.h>

// std lib headers
#include <string>
#include <vector>
#include <memory>

namespace VulkanTutorial 
{
    class EngineSwapChain 
    {
    public:

        EngineSwapChain(EngineDevice& deviceRef, VkExtent2D extent);
        EngineSwapChain(EngineDevice& deviceRef, VkExtent2D extent, std::shared_ptr<EngineSwapChain> Previous);
        virtual ~EngineSwapChain();

        EngineSwapChain(const EngineSwapChain&) = delete;
        EngineSwapChain& operator = (const EngineSwapChain&) = delete;

        EngineSwapChain(EngineSwapChain&&) = delete;
        EngineSwapChain& operator = (EngineSwapChain&&) = delete;

        VkFramebuffer GetFrameBuffer(int index) { return m_SwapChainFramebuffers[index]; }
        VkRenderPass GetRenderPass() { return m_RenderPass; }
        VkImageView GetImageView(int index) { return m_SwapChainImageViews[index]; }
        size_t ImageCount() { return m_SwapChainImages.size(); }
        VkFormat GetSwapChainImageFormat() { return m_SwapChainImageFormat; }
        VkExtent2D GetSwapChainExtent() { return m_SwapChainExtent; }
        uint32_t Width() { return m_SwapChainExtent.width; }
        uint32_t Height() { return m_SwapChainExtent.height; }

        float ExtentAspectRatio() {   return static_cast<float>(m_SwapChainExtent.width) / static_cast<float>(m_SwapChainExtent.height);  }
        VkFormat FindDepthFormat();

        VkResult AcquireNextImage(uint32_t *imageIndex);
        VkResult SubmitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *imageIndex);

    private:
        void Init();
        void CreateSwapChain();
        void CreateImageViews();
        void CreateDepthResources();
        void CreateRenderPass();
        void CreateFramebuffers();
        void CreateSyncObjects();

        // Helper functions
        VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
        VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
        VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

        EngineDevice& m_Device;
        const VkExtent2D m_WindowExtent;

        VkFormat m_SwapChainImageFormat;
        VkExtent2D m_SwapChainExtent;

        std::vector<VkFramebuffer> m_SwapChainFramebuffers;
        VkRenderPass m_RenderPass;

        std::vector<VkImage> m_DepthImages;
        std::vector<VkDeviceMemory> m_DepthImageMemorys;
        std::vector<VkImageView> m_DepthImageViews;
        std::vector<VkImage> m_SwapChainImages;
        std::vector<VkImageView> m_SwapChainImageViews;

        VkSwapchainKHR m_SwapChain;
        std::shared_ptr<EngineSwapChain> m_OldSwapChain;

        std::vector<VkSemaphore> m_ImageAvailableSemaphores;
        std::vector<VkSemaphore> m_RenderFinishedSemaphores;
        std::vector<VkFence> m_InFlightFences;
        std::vector<VkFence> m_ImagesInFlight;
        size_t m_CurrentFrame = 0;
        };

}  

#endif //__EngineSwapChain_h__
