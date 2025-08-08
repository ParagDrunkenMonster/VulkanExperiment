#include "Mesh.h"
#include <cassert>
#include <cstring>

namespace VulkanTutorial
{
	Mesh::Mesh(EngineDevice& Device, const std::vector<Mesh::Vertex>& Vertices)
		: m_Device(Device)
	{
		CreateVertexBuffer(Vertices);
	}

	Mesh::~Mesh()
	{
		vkDestroyBuffer(m_Device.Device(), m_VertexBuffer, nullptr);
		vkFreeMemory(m_Device.Device(), m_VertexBufferMemory, nullptr);
	}

	void Mesh::Bind(VkCommandBuffer CommandBuffer)
	{
		VkBuffer Buffers[] = { m_VertexBuffer };
		VkDeviceSize Offsets[] = { 0 };
		vkCmdBindVertexBuffers(CommandBuffer, 0, 1, Buffers, Offsets);
	}

	void Mesh::Draw(VkCommandBuffer CommandBuffer)
	{
		vkCmdDraw(CommandBuffer, m_VertexCount, 1, 0, 0);
	}

	void Mesh::CreateVertexBuffer(const std::vector<Vertex>& Vertices)
	{
		m_VertexCount = (uint32_t)Vertices.size();
		assert(m_VertexCount >= 3 && "Vertex count should be at least 3");

		VkDeviceSize BufferSize = sizeof(Vertices[0]) * m_VertexCount;

		m_Device.CreateBuffer(BufferSize
			, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			, m_VertexBuffer, m_VertexBufferMemory);

		void* Data;
		vkMapMemory(m_Device.Device(), m_VertexBufferMemory, 0, BufferSize, 0, &Data);
		memcpy(Data, Vertices.data(), (size_t)BufferSize);
		vkUnmapMemory(m_Device.Device(), m_VertexBufferMemory);
	}

	std::vector<VkVertexInputBindingDescription> Mesh::Vertex::GetBindingDescriptions()
	{
		std::vector< VkVertexInputBindingDescription> BindingDescriptions(1);
		BindingDescriptions[0].binding = 0;
		BindingDescriptions[0].stride = sizeof(Vertex);
		BindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return BindingDescriptions;
	}

	std::vector<VkVertexInputAttributeDescription> Mesh::Vertex::GetAttributeDescriptions()
	{
		std::vector<VkVertexInputAttributeDescription> AttributeDescriptions(2);
		AttributeDescriptions[0].binding = 0;
		AttributeDescriptions[0].location = 0;
		AttributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		AttributeDescriptions[0].offset = offsetof(Vertex, position);

		AttributeDescriptions[1].binding = 0;
		AttributeDescriptions[1].location = 1;
		AttributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		AttributeDescriptions[1].offset = offsetof(Vertex, color);

		return AttributeDescriptions;
	}
}