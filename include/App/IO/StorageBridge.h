
#pragma once
#include <string>
#include <vector>
#include <variant>
#include <memory>
#include <filesystem>
#include <string_view>
#include <cstdio>
#include <queue>
#include <oneapi/tbb/concurrent_queue.h>
#include <unordered_map>
#include <mutex>
#include <memory>
#include <fmt/format.h>
#include <array>

#include "Misc/EnumClass.h"
#include "Misc/Types.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/Gender.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/Species/LifeStage.h"
#include "App/Model/IBM/Landscape/LivingBeings/Animals/ActivityType.h"
#include "App/Model/IBM/Landscape/LivingBeings/TimeUnits.h"


namespace StorageBridge {
	inline bool diskOutputMockEnabled = false;

	inline std::unordered_map<std::string, std::FILE*> openFiles;

	inline void setDiskOutputMockEnabled(bool enabled) {
		diskOutputMockEnabled = enabled;
	}


	void writeHeaderPackToDisk(const std::filesystem::path& filePath, const std::string& header);

	void writeContentPackToDisk(const std::filesystem::path& filePath, fmt::memory_buffer& formattedTextBlock);

	void writeClosePackToDisk(const std::filesystem::path& filePath);

	void writeFullFilePackToDisk(const std::filesystem::path& filePath, const std::string& header, fmt::memory_buffer& formattedTextBlock);

	void writePackToDisk(
		const std::filesystem::path& filePath, const std::string& header, 
		bool closeAfterWrite, fmt::memory_buffer& formattedTextBlock
	);
}
