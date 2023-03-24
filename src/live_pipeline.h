#pragma once

#include <string>
#include <vector>

namespace live {
	class LivePipeline {
	public:
		LivePipeline(const std::string& vertFilePath, const std::string& fragFilePath);

	private:
		static std::vector<char> readFile(const std::string& filePath);

		void createGraphicsPipeline(const std::string& vertFilePath, const std::string& fragFilePath);
	};
}