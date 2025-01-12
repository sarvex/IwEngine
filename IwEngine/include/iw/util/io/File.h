#pragma once

#include "iw/util/iwutil.h"
#include <string>
#include <vector>

namespace iw {
namespace util {
	IWUTIL_API
	bool WriteFile(
		const std::string& filePath,
		const std::string& contents);

	IWUTIL_API
	std::string ReadFile(
		const std::string& filePath);

	IWUTIL_API
	std::vector<std::string> ReadFileLines(
		const std::string& filePath);

	IWUTIL_API
	bool FileExists(
		const std::string& filePath);

	IWUTIL_API
	uintmax_t GetFileSize(
		const std::string& filePath);
}

	using namespace util;
}
