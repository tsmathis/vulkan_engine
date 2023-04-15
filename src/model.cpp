#include "model.h"
#include "utility.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#define GLM_EXPERIMENTAL_ENABLE
#include <glm/gtx/hash.hpp>

#include <cassert>
#include <cstring>
#include <unordered_map>


namespace std {
	template<>
	struct hash<live::Model::Vertex> {
		size_t operator()(live::Model::Vertex const& vertex) const {
			size_t seed = 0;
			live::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
			return seed;
		}
	};
}


live::Model::Model(LiveDevice& device, const Model::Builder& builder) : liveDevice{ device } {
	createVertexBuffers(builder.vertices);
	createIndexBuffers(builder.indices);
}

live::Model::~Model() {
	vkDestroyBuffer(liveDevice.device(), vertexBuffer, nullptr);
	vkFreeMemory(liveDevice.device(), vertexBufferMemory, nullptr);

	if (hasIndexBuffer) {
		vkDestroyBuffer(liveDevice.device(), indexBuffer, nullptr);
		vkFreeMemory(liveDevice.device(), indexBufferMemory, nullptr);
	}
}

std::unique_ptr<live::Model> live::Model::createModelFromFile(LiveDevice& device, const std::string& filepath) {
	Builder builder{};
	builder.loadModels(filepath);
	return std::make_unique<Model>(device, builder);
}

void live::Model::bind(VkCommandBuffer commandBuffer) {
	VkBuffer buffers[] = { vertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

	if (hasIndexBuffer) {
		vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
	}
}

void live::Model::draw(VkCommandBuffer commandBuffer) {
	if (hasIndexBuffer) {
		vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
	} else {
	vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
	}
}

void live::Model::createVertexBuffers(const std::vector<Vertex>& vertices) {
	vertexCount = static_cast<uint32_t>(vertices.size());
	assert(vertexCount >= 3 && "Vertex count must be three or greater");

	VkDeviceSize   bufferSize = sizeof(vertices[0]) * vertexCount;
	VkBuffer       stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	liveDevice.createBuffer(
		bufferSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer,
		stagingBufferMemory
	);

	void* data;
	vkMapMemory(liveDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
	vkUnmapMemory(liveDevice.device(), stagingBufferMemory);

	liveDevice.createBuffer(
		bufferSize,
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		vertexBuffer,
		vertexBufferMemory
	);

	liveDevice.copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

	vkDestroyBuffer(liveDevice.device(), stagingBuffer, nullptr);
	vkFreeMemory(liveDevice.device(), stagingBufferMemory, nullptr);
}

void live::Model::createIndexBuffers(const std::vector<uint32_t>& indices) {
	indexCount = static_cast<uint32_t>(indices.size());
	hasIndexBuffer = indexCount > 0;

	if (!hasIndexBuffer) {
		return;
	}

	VkDeviceSize   bufferSize = sizeof(indices[0]) * indexCount;
	VkBuffer       stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	liveDevice.createBuffer(
		bufferSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		stagingBuffer,
		stagingBufferMemory
	);

	void* data;
	vkMapMemory(liveDevice.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
	vkUnmapMemory(liveDevice.device(), stagingBufferMemory);

	liveDevice.createBuffer(
		bufferSize,
		VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		indexBuffer,
		indexBufferMemory
	);

	liveDevice.copyBuffer(stagingBuffer, indexBuffer, bufferSize);

	vkDestroyBuffer(liveDevice.device(), stagingBuffer, nullptr);
	vkFreeMemory(liveDevice.device(), stagingBufferMemory, nullptr);
}

std::vector<VkVertexInputBindingDescription> live::Model::Vertex::getBindingDescriptions() {
	std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
	bindingDescriptions[0].binding   = 0;
	bindingDescriptions[0].stride    = sizeof(Vertex);
	bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return bindingDescriptions;
}

std::vector<VkVertexInputAttributeDescription> live::Model::Vertex::getAttributeDescriptions() {
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

	attributeDescriptions.push_back({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position) });
	attributeDescriptions.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) });
	attributeDescriptions.push_back({ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) });
	attributeDescriptions.push_back({ 3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, uv) });
						  
	return attributeDescriptions;
}

void live::Model::Builder::loadModels(const std::string& filepame) {
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepame.c_str())) {
		throw std::runtime_error(warn + err);
	}

	vertices.clear();
	indices.clear();

	std::unordered_map<Vertex, uint32_t> uniqueVertices{};

	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			Vertex vertex{};

			if (index.vertex_index >= 0) {
				vertex.position = {
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				};

				vertex.color = {
					attrib.colors[3 * index.vertex_index + 0],
					attrib.colors[3 * index.vertex_index + 1],
					attrib.colors[3 * index.vertex_index + 2]
				};
			}

			if (index.normal_index >= 0) {
				vertex.normal = {
					attrib.normals[3 * index.normal_index + 0],
					attrib.normals[3 * index.normal_index + 1],
					attrib.normals[3 * index.normal_index + 2]
				};
			}

			if (index.texcoord_index >= 0) {
				vertex.uv = {
					attrib.texcoords[2 * index.texcoord_index + 0],
					attrib.texcoords[2 * index.texcoord_index + 1]
				};
			}

			if (uniqueVertices.count(vertex) == 0) {
				uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
				vertices.push_back(vertex);
			}

			indices.push_back(uniqueVertices[vertex]);

		}
	}
}
