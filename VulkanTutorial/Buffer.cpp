#include "Buffer.h"

#include <cassert>
#include <cstring>

namespace VulkanTutorial
{
    VkDeviceSize Buffer::GetAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment) 
    {
        if (minOffsetAlignment > 0) 
        {
            return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);
        }
        return instanceSize;
    }

    Buffer::Buffer(EngineDevice& device, VkDeviceSize instanceSize, uint32_t instanceCount, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize minOffsetAlignment)
        : m_EngineDevice(device)
        , m_InstanceSize(instanceSize)
        , m_InstanceCount(instanceCount)
        , m_UsageFlags(usageFlags)
        , m_MemoryPropertyFlags{ memoryPropertyFlags } 
    {
        m_AlignmentSize = GetAlignment(instanceSize, minOffsetAlignment);
        m_BufferSize = m_AlignmentSize * instanceCount;
        device.CreateBuffer(m_BufferSize, usageFlags, m_MemoryPropertyFlags, m_Buffer, m_Memory);
    }

    Buffer::~Buffer() 
    {
        Unmap();
        vkDestroyBuffer(m_EngineDevice.Device(), m_Buffer, nullptr);
        vkFreeMemory(m_EngineDevice.Device(), m_Memory, nullptr);
    }

    /**
     * Map a m_Memory range of this m_Buffer. If successful, m_Mapped points to the specified m_Buffer range.
     *
     * @param size (Optional) Size of the m_Memory range to Map. Pass VK_WHOLE_SIZE to Map the complete
     * m_Buffer range.
     * @param offset (Optional) Byte offset from beginning
     *
     * @return VkResult of the m_Buffer mapping call
     */
    VkResult Buffer::Map(VkDeviceSize size, VkDeviceSize offset) 
    {
        assert(m_Buffer && m_Memory && "Called Map on m_Buffer before create");
        return vkMapMemory(m_EngineDevice.Device(), m_Memory, offset, size, 0, &m_Mapped);
    }

    /**
     * Unmap a m_Mapped m_Memory range
     *
     * @note Does not return a result as vkUnmapMemory can't fail
     */
    void Buffer::Unmap() 
    {
        if (m_Mapped)
        {
            vkUnmapMemory(m_EngineDevice.Device(), m_Memory);
            m_Mapped = nullptr;
        }
    }

    /**
     * Copies the specified data to the m_Mapped m_Buffer. Default value writes whole m_Buffer range
     *
     * @param data Pointer to the data to copy
     * @param size (Optional) Size of the data to copy. Pass VK_WHOLE_SIZE to Flush the complete m_Buffer
     * range.
     * @param offset (Optional) Byte offset from beginning of m_Mapped region
     *
     */
    void Buffer::WriteToBuffer(void* data, VkDeviceSize size, VkDeviceSize offset) 
    {
        assert(m_Mapped && "Cannot copy to unmapped m_Buffer");

        if (size == VK_WHOLE_SIZE) 
        {
            memcpy(m_Mapped, data, m_BufferSize);
        }
        else 
        {
            char* memOffset = (char*)m_Mapped;
            memOffset += offset;
            memcpy(memOffset, data, size);
        }
    }

    /**
     * Flush a m_Memory range of the m_Buffer to make it visible to the device
     *
     * @note Only required for non-coherent m_Memory
     *
     * @param size (Optional) Size of the m_Memory range to Flush. Pass VK_WHOLE_SIZE to Flush the
     * complete m_Buffer range.
     * @param offset (Optional) Byte offset from beginning
     *
     * @return VkResult of the Flush call
     */
    VkResult Buffer::Flush(VkDeviceSize size, VkDeviceSize offset) 
    {
        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = m_Memory;
        mappedRange.offset = offset;
        mappedRange.size = size;
        return vkFlushMappedMemoryRanges(m_EngineDevice.Device(), 1, &mappedRange);
    }

    /**
     * Invalidate a m_Memory range of the m_Buffer to make it visible to the host
     *
     * @note Only required for non-coherent m_Memory
     *
     * @param size (Optional) Size of the m_Memory range to Invalidate. Pass VK_WHOLE_SIZE to Invalidate
     * the complete m_Buffer range.
     * @param offset (Optional) Byte offset from beginning
     *
     * @return VkResult of the Invalidate call
     */
    VkResult Buffer::Invalidate(VkDeviceSize size, VkDeviceSize offset) 
    {
        VkMappedMemoryRange mappedRange = {};
        mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        mappedRange.memory = m_Memory;
        mappedRange.offset = offset;
        mappedRange.size = size;
        return vkInvalidateMappedMemoryRanges(m_EngineDevice.Device(), 1, &mappedRange);
    }

    /**
     * Create a m_Buffer info descriptor
     *
     * @param size (Optional) Size of the m_Memory range of the descriptor
     * @param offset (Optional) Byte offset from beginning
     *
     * @return VkDescriptorBufferInfo of specified offset and range
     */
    VkDescriptorBufferInfo Buffer::DescriptorInfo(VkDeviceSize size, VkDeviceSize offset) 
    {
        return VkDescriptorBufferInfo{m_Buffer, offset, size };
    }

    /**
     * Copies "m_InstanceSize" bytes of data to the m_Mapped m_Buffer at an offset of index * m_AlignmentSize
     *
     * @param data Pointer to the data to copy
     * @param index Used in offset GetUsageFlags
     *
     */
    void Buffer::WriteToIndex(void* data, int index) 
    {
        WriteToBuffer(data, m_InstanceSize, index * m_AlignmentSize);
    }

    /**
     *  Flush the m_Memory range at index * m_AlignmentSize of the m_Buffer to make it visible to the device
     *
     * @param index Used in offset GetUsageFlags
     *
     */
    VkResult Buffer::FlushIndex(int index) { return Flush(m_AlignmentSize, index * m_AlignmentSize); }

    /**
     * Create a m_Buffer info descriptor
     *
     * @param index Specifies the region given by index * m_AlignmentSize
     *
     * @return VkDescriptorBufferInfo for instance at index
     */
    VkDescriptorBufferInfo Buffer::DescriptorInfoForIndex(int index) 
    {
        return DescriptorInfo(m_AlignmentSize, index * m_AlignmentSize);
    }

    /**
     * Invalidate a m_Memory range of the m_Buffer to make it visible to the host
     *
     * @note Only required for non-coherent m_Memory
     *
     * @param index Specifies the region to Invalidate: index * m_AlignmentSize
     *
     * @return VkResult of the Invalidate call
     */
    VkResult Buffer::InvalidateIndex(int index) 
    {
        return Invalidate(m_AlignmentSize, index * m_AlignmentSize);
    }

}