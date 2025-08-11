#ifndef __EngineDevice_h__
#define __EngineDevice_h__

#include "MyWindow.h"

#include <string>
#include <vector>

namespace VulkanTutorial 
{
    struct SwapChainSupportDetails 
    {
        VkSurfaceCapabilitiesKHR Capabilities;
        std::vector<VkSurfaceFormatKHR> Formats;
        std::vector<VkPresentModeKHR> PresentModes;
    };

    struct QueueFamilyIndices 
    {
        uint32_t GraphicsFamily;
        uint32_t PresentFamily;
        bool GraphicsFamilyHasValue = false;
        bool PresentFamilyHasValue = false;
        bool IsComplete() { return GraphicsFamilyHasValue && PresentFamilyHasValue; }
    };

    class EngineDevice 
    {
    public:
#ifdef NDEBUG
        static const bool EnableValidationLayers = false;
#else
        static const bool EnableValidationLayers = true;
#endif

        EngineDevice(MyWindow &Window);
        ~EngineDevice();

        EngineDevice(const EngineDevice&) = delete;
        EngineDevice& operator = (const EngineDevice&) = delete;
        EngineDevice(EngineDevice&&) = delete;
        EngineDevice& operator = (EngineDevice &&) = delete;

        VkCommandPool GetCommandPool() { return m_CommandPool; }
        VkDevice Device() { return m_Device; }
        VkSurfaceKHR Surface() { return m_Surface; }
        VkQueue GraphicsQueue() { return m_GraphicsQueue; }
        VkQueue PresentQueue() { return m_PresentQueue; }

        SwapChainSupportDetails GetSwapChainSupport() { return QuerySwapChainSupport(m_PhysicalDevice); }
        uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags memoryProperties);
        QueueFamilyIndices FindPhysicalQueueFamilies() { return FindQueueFamilies(m_PhysicalDevice); }
        VkFormat FindSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

        // Buffer Helper Functions
        void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties, VkBuffer &buffer, VkDeviceMemory &bufferMemory);
        VkCommandBuffer BeginSingleTimeCommands();
        void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
        void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
        void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layerCount);

        void CreateImageWithInfo(const VkImageCreateInfo &imageInfo, VkMemoryPropertyFlags memoryProperties, VkImage &image, VkDeviceMemory &imageMemory);

        const VkPhysicalDeviceProperties& PhysicalDeviceProperties() const { return m_PhysicalDeviceProperties; }
    
    private:

        void CreateInstance();
        void SetupDebugMessenger();
        void CreateSurface();
        void PickPhysicalDevice();
        void CreateLogicalDevice();
        void CreateCommandPool();

        // helper functions
        bool IsDeviceSuitable(VkPhysicalDevice device);
        std::vector<const char *> GetRequiredExtensions();
        bool CheckValidationLayerSupport();
        QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
        void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);
        void HasGflwRequiredInstanceExtensions();
        bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
        SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);

        VkPhysicalDeviceProperties m_PhysicalDeviceProperties;

        VkInstance m_VKInstance;
        VkDebugUtilsMessengerEXT m_DebugMessenger;
        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
        MyWindow &m_Window;
        VkCommandPool m_CommandPool;

        VkDevice m_Device;
        VkSurfaceKHR m_Surface;
        VkQueue m_GraphicsQueue;
        VkQueue m_PresentQueue;

        const std::vector<const char *> m_ValidationLayers = {"VK_LAYER_KHRONOS_validation"};
        const std::vector<const char *> m_DeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};
    };

} 

#endif //__EngineDevice_h__