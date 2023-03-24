#include "live_pipeline.h"

#include <fstream>
#include <iostream>
#include <stdexcept>


live::LivePipeline::LivePipeline(const std::string& vertFilePath, const std::string& fragFilePath) {
	createGraphicsPipeline(vertFilePath, fragFilePath);
}

std::vector<char> live::LivePipeline::readFile(const std::string& filePath) {
	std::ifstream file(filePath, std::ios::ate | std::ios::binary);
	
	if (!file.is_open()) {
		throw std::runtime_error("Failed to open file: " + filePath);
	}

	size_t fileSize = static_cast<size_t>(file.tellg());
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);
	file.close();
	
	return buffer;
}

void live::LivePipeline::createGraphicsPipeline(const std::string& vertFilePath, const std::string& fragFilePath) {
	auto vertCode = readFile(vertFilePath);
	auto fragCode = readFile(fragFilePath);

	std::cout << "Vertex Shader Code size: " << vertCode.size() << '\n';
	std::cout << "Fragement Shader Code size: " << fragCode.size() << '\n';
}
