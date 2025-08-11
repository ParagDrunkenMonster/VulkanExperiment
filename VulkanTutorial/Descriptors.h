#ifndef __Descriptors_h__
#define __Descriptors_h__

#include "EngineDevice.h"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace VulkanTutorial
{
    class DescriptorSetLayout 
    {
        friend class DescriptorWriter;

    public:
        class Builder 
        {
        public:

            Builder(EngineDevice& engineDevice) 
                : m_EngineDevice(engineDevice) 
            {
            }

            Builder& AddBinding(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, uint32_t count = 1);
            std::unique_ptr<DescriptorSetLayout> Build() const;

        private:

            EngineDevice& m_EngineDevice;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_Bindings{};
        };

        DescriptorSetLayout(EngineDevice& engineDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~DescriptorSetLayout();

        DescriptorSetLayout(const DescriptorSetLayout&) = delete;
        DescriptorSetLayout& operator = (const DescriptorSetLayout&) = delete;

        DescriptorSetLayout(DescriptorSetLayout&&) = delete;
        DescriptorSetLayout& operator = (DescriptorSetLayout&&) = delete;

        VkDescriptorSetLayout GetDescriptorSetLayout() const { return m_DescriptorSetLayout; }

    private:

        EngineDevice& m_EngineDevice;
        VkDescriptorSetLayout m_DescriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> m_Bindings;
    };

    class DescriptorPool 
    {
        friend class DescriptorWriter;

    public:
        class Builder 
        {
        public:
            Builder(EngineDevice& engineDevice) 
                : m_EngineDevice(engineDevice)
            {
            }

            Builder& AddPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder& SetPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder& SetMaxSets(uint32_t count);
            std::unique_ptr<DescriptorPool> Build() const;

        private:

            EngineDevice& m_EngineDevice;
            std::vector<VkDescriptorPoolSize> m_PoolSizes{};
            uint32_t m_MaxSets = 1000;
            VkDescriptorPoolCreateFlags m_PoolFlags = 0;
        };

        DescriptorPool(EngineDevice& engineDevice, uint32_t maxSets, VkDescriptorPoolCreateFlags poolFlags, const std::vector<VkDescriptorPoolSize>& poolSizes);
        ~DescriptorPool();

        DescriptorPool(const DescriptorPool&) = delete;
        DescriptorPool& operator = (const DescriptorPool&) = delete;

        DescriptorPool(DescriptorPool&&) = delete;
        DescriptorPool& operator = (DescriptorPool&&) = delete;

        bool AllocateDescriptor(const VkDescriptorSetLayout m_DescriptorSetLayout, VkDescriptorSet& descriptor) const;

        void FreeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

        void ResetPool();

    private:

        EngineDevice& m_EngineDevice;
        VkDescriptorPool m_DescriptorPool;
    };

    class DescriptorWriter 
    {
    public:

        DescriptorWriter(DescriptorSetLayout& setLayout, DescriptorPool& pool);

        DescriptorWriter& WriteBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
        DescriptorWriter& WriteImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

        bool Build(VkDescriptorSet& set);
        void Overwrite(VkDescriptorSet& set);

    private:

        DescriptorSetLayout& m_SetLayout;
        DescriptorPool& m_Pool;
        std::vector<VkWriteDescriptorSet> m_Writes;
    };
}

#endif //__Descriptors_h__
