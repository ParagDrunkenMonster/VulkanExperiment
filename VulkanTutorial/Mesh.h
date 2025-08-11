#ifndef __Mesh_h__
#define __Mesh_h__

#include "EngineDevice.h"
#include "Buffer.h"
#include <glm/glm.hpp>
#include <vector>

#include <memory>

namespace VulkanTutorial
{
	class Mesh
	{
	public:

		struct Vertex
		{
			glm::vec3 position;
			glm::vec3 color;
			glm::vec3 normal;
			glm::vec2 uv;

			static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
		};

		struct Builder
		{
			std::vector<Vertex> Vertices;
			std::vector<uint32_t> Indices;

			void LoadModel(const std::string& FilePath);
		};

		Mesh(EngineDevice& Device, const Builder& MeshBuilder);
		virtual ~Mesh();

		Mesh(const Mesh&) = delete;
		Mesh& operator = (const Mesh&) = delete;

		Mesh(Mesh&&) = delete;
		Mesh& operator = (Mesh&&) = delete;

		static std::unique_ptr<Mesh> CreateModelFromFile(EngineDevice& Device, const std::string& FilePath);

		void Bind(VkCommandBuffer CommandBuffer);
		void Draw(VkCommandBuffer CommandBuffer);

	private:

		void CreateVertexBuffer(const std::vector<Vertex>& Vertices);
		void CreateIndexBuffer(const std::vector<uint32_t>& Indices);

		EngineDevice& m_Device;

		std::unique_ptr<Buffer> m_VertexBuffer;
		uint32_t m_VertexCount;

		bool m_HasIndexBuffer;
		std::unique_ptr<Buffer> m_IndexBuffer;
		uint32_t m_IndexCount;
	};
}

#endif //__Mesh_h__
