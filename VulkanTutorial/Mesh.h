#ifndef __Mesh_h__
#define __Mesh_h__

#include "EngineDevice.h"

#include <glm/glm.hpp>
#include <vector>

namespace VulkanTutorial
{
	class Mesh
	{
	public:

		struct Vertex
		{
			glm::vec2 position;
			glm::vec3 color;

			static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
		};

		Mesh(EngineDevice& Device, const std::vector<Vertex>& Vertices);
		virtual ~Mesh();

		Mesh(const Mesh&) = delete;
		Mesh& operator = (const Mesh&) = delete;

		Mesh(Mesh&&) = delete;
		Mesh& operator = (Mesh&&) = delete;

		void Bind(VkCommandBuffer CommandBuffer);
		void Draw(VkCommandBuffer CommandBuffer);

	private:

		void CreateVertexBuffer(const std::vector<Vertex>& Vertices);

		EngineDevice& m_Device;
		VkBuffer m_VertexBuffer;
		VkDeviceMemory m_VertexBufferMemory;
		uint32_t m_VertexCount;
	};
}

#endif //__Mesh_h__
