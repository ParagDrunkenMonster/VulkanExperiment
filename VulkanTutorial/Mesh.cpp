#include "Mesh.h"
#include <cassert>
#include <cstring>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <iostream>

namespace VulkanTutorial
{
	Mesh::Mesh(EngineDevice& Device, const Builder& MeshBuilder)
		: m_Device(Device)
		, m_HasIndexBuffer(false)
	{
		CreateVertexBuffer(MeshBuilder.Vertices);
		CreateIndexBuffer(MeshBuilder.Indices);
	}

	Mesh::~Mesh()
	{
		vkDestroyBuffer(m_Device.Device(), m_VertexBuffer, nullptr);
		vkFreeMemory(m_Device.Device(), m_VertexBufferMemory, nullptr);

		if (m_HasIndexBuffer)
		{
			vkDestroyBuffer(m_Device.Device(), m_IndexBuffer, nullptr);
			vkFreeMemory(m_Device.Device(), m_IndexBufferMemory, nullptr);
		}
	}

	void Mesh::Bind(VkCommandBuffer CommandBuffer)
	{
		VkBuffer Buffers[] = { m_VertexBuffer };
		VkDeviceSize Offsets[] = { 0 };
		vkCmdBindVertexBuffers(CommandBuffer, 0, 1, Buffers, Offsets);

		if (m_HasIndexBuffer)
			vkCmdBindIndexBuffer(CommandBuffer, m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);
	}

	void Mesh::Draw(VkCommandBuffer CommandBuffer)
	{
		if (m_HasIndexBuffer)
			vkCmdDrawIndexed(CommandBuffer, m_IndexCount, 1, 0, 0, 0);
		else
			vkCmdDraw(CommandBuffer, m_VertexCount, 1, 0, 0);
	}


	// Old implementation, slower
	/*void Mesh::CreateVertexBuffer(const std::vector<Vertex>& Vertices)
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

	void Mesh::CreateIndexBuffer(const std::vector<uint32_t>& Indices)
	{
		m_IndexCount = (uint32_t)Indices.size();

		if (m_IndexCount > 0)
		{
			VkDeviceSize BufferSize = sizeof(Indices[0]) * m_IndexCount;

			m_Device.CreateBuffer(BufferSize
				, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
				, m_IndexBuffer, m_IndexBufferMemory);

			void* Data;
			vkMapMemory(m_Device.Device(), m_IndexBufferMemory, 0, BufferSize, 0, &Data);
			memcpy(Data, Indices.data(), (size_t)BufferSize);
			vkUnmapMemory(m_Device.Device(), m_IndexBufferMemory);

			m_HasIndexBuffer = true;
		}
	}*/

	void Mesh::CreateVertexBuffer(const std::vector<Vertex>& Vertices)
	{
		m_VertexCount = (uint32_t)Vertices.size();
		assert(m_VertexCount >= 3 && "Vertex count should be at least 3");

		VkDeviceSize BufferSize = sizeof(Vertices[0]) * m_VertexCount;

		VkBuffer StagingBuffer;
		VkDeviceMemory StagingBufferMemory;

		m_Device.CreateBuffer(BufferSize
			, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
			, StagingBuffer, StagingBufferMemory);

		void* Data;
		vkMapMemory(m_Device.Device(), StagingBufferMemory, 0, BufferSize, 0, &Data);
		memcpy(Data, Vertices.data(), (size_t)BufferSize);
		vkUnmapMemory(m_Device.Device(), StagingBufferMemory);

		m_Device.CreateBuffer(BufferSize
			, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
			, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
			, m_VertexBuffer, m_VertexBufferMemory);

		m_Device.CopyBuffer(StagingBuffer, m_VertexBuffer, BufferSize);

		vkDestroyBuffer(m_Device.Device(), StagingBuffer, nullptr);
		vkFreeMemory(m_Device.Device(), StagingBufferMemory, nullptr);
	}

	void Mesh::CreateIndexBuffer(const std::vector<uint32_t>& Indices)
	{
		m_IndexCount = (uint32_t)Indices.size();

		if (m_IndexCount > 0)
		{
			VkDeviceSize BufferSize = sizeof(Indices[0]) * m_IndexCount;

			VkBuffer StagingBuffer;
			VkDeviceMemory StagingBufferMemory;

			m_Device.CreateBuffer(BufferSize
				, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
				, StagingBuffer, StagingBufferMemory);

			void* Data;
			vkMapMemory(m_Device.Device(), StagingBufferMemory, 0, BufferSize, 0, &Data);
			memcpy(Data, Indices.data(), (size_t)BufferSize);
			vkUnmapMemory(m_Device.Device(), StagingBufferMemory);

			m_Device.CreateBuffer(BufferSize
				, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT
				, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
				, m_IndexBuffer, m_IndexBufferMemory);

			m_Device.CopyBuffer(StagingBuffer, m_IndexBuffer, BufferSize);

			vkDestroyBuffer(m_Device.Device(), StagingBuffer, nullptr);
			vkFreeMemory(m_Device.Device(), StagingBufferMemory, nullptr);

			m_HasIndexBuffer = true;
		}
	}

	std::unique_ptr<Mesh> Mesh::CreateModelFromFile(EngineDevice& Device, const std::string& FilePath)
	{
		Builder MeshBuilder{};
		MeshBuilder.LoadModel(FilePath);

		std::cout << "Vertex Count :  " << MeshBuilder.Vertices.size() << std::endl;

		return std::make_unique<Mesh>(Device, MeshBuilder);
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
		AttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		AttributeDescriptions[0].offset = offsetof(Vertex, position);

		AttributeDescriptions[1].binding = 0;
		AttributeDescriptions[1].location = 1;
		AttributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		AttributeDescriptions[1].offset = offsetof(Vertex, color);

		return AttributeDescriptions;
	}

	void Mesh::Builder::LoadModel(const std::string& FilePath)
	{
		tinyobj::attrib_t Attrib;
		std::vector<tinyobj::shape_t> Shapes;
		std::vector<tinyobj::material_t> Materials;

		std::string Warn, Err;

		if (!tinyobj::LoadObj(&Attrib, &Shapes, &Materials, &Warn, &Err, FilePath.c_str()))
		{
			throw std::runtime_error(Warn + Err);
		}

		Vertices.clear();
		Indices.clear();

		for (const auto& Shape : Shapes)
		{
			for (const auto& Index : Shape.mesh.indices)
			{
				Vertex Vert{};
				if (Index.vertex_index >= 0) 
				{
					Vert.position = {
						Attrib.vertices[3 * Index.vertex_index + 0],
						Attrib.vertices[3 * Index.vertex_index + 1],
						Attrib.vertices[3 * Index.vertex_index + 2]
					};
				}

				Vert.color = {1.0f, 1.0f, 1.0f};

				if (Index.normal_index >= 0) 
				{
					Vert.normal = {
						Attrib.normals[3 * Index.normal_index + 0],
						Attrib.normals[3 * Index.normal_index + 1],
						Attrib.normals[3 * Index.normal_index + 2]
					};
				}

				if (Index.texcoord_index >= 0) 
				{
					Vert.uv = {
						Attrib.texcoords[2 * Index.texcoord_index + 0],
						Attrib.texcoords[2 * Index.texcoord_index + 1],
					};
				}

				Vertices.push_back(Vert);
			}
		}
	}
}