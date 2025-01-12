#include "iw/util/io/File.h"
#include "iw/log/logger.h"
#include <cstdio>
#include <cerrno>
#include <string>
#include <algorithm>

#define MAX_LINE_LENGTH 1000

namespace iw {
namespace util {
	static void ReportError(
		int errerno,
		const std::string& info)
	{
		std::string error;
		error.resize(256);

		strerror_s(&error[0], 100, errerno);

		size_t i = error.find('\0');
		error = error.substr(0, i);
		error.append(" (");
		error.append(std::to_string(errerno));
		error.append("): ");
		error.append(info);

		LOG_WARNING << error;
	}

	bool WriteFile(
		const std::string& filePath,
		const std::string& contents)
	{
		FILE* file;
		errno_t errorCode = fopen_s(&file, filePath.c_str(), "wb");
		if (file) {
			fseek(file, 0, SEEK_END);
			rewind(file);
			fwrite(&contents[0], 1, contents.size(), file);
			fclose(file);
		}

		else {
			ReportError(errorCode, filePath);
		}

		return file != nullptr;
	}

	std::string ReadFile(
		const std::string& filePath)
	{
		std::string contents;
		FILE* file;
		errno_t errorCode = fopen_s(&file, filePath.c_str(), "rb");
		if (file) {
			fseek(file, 0, SEEK_END);
			contents.resize(ftell(file));
			rewind(file);
			fread(&contents[0], 1, contents.size(), file);
			fclose(file);
		}

		else {
			ReportError(errorCode, filePath);
		}

		// kinda sucks

		contents.erase(std::remove(contents.begin(), contents.end(), '\r'), contents.end());

		return contents;
	}

	std::vector<std::string> ReadFileLines(
		const std::string& filePath)
	{
		std::vector<std::string> lines;
		FILE* file;
		errno_t errorCode = fopen_s(&file, filePath.c_str(), "rt");
		if (file) {
			size_t lineCount = 0;
			for (char c = getc(file); c != EOF; c = getc(file)) {
				if (c == '\n') lineCount++;
			}

			rewind(file);
			lines.reserve(lineCount + 1);

			char line[MAX_LINE_LENGTH];
			while (fgets(line, MAX_LINE_LENGTH, file)) {
				char& last = line[strlen(line) - 1];
				if (last == '\n') {
					last = '\0';
				}

				lines.push_back(line);
			}

			fclose(file);
		}

		else {
			ReportError(errorCode, filePath);
		}


		return lines;
	}

	bool FileExists(
		const std::string& filePath)
	{
		FILE* file;
		fopen_s(&file, filePath.c_str(), "rb");
		if (file) {
			fclose(file);
		}

		return file;
	}

	uintmax_t GetFileSize(
		const std::string& filePath)
	{
		uintmax_t bytes = 0u;
		FILE* file;
		errno_t errorCode = fopen_s(&file, filePath.c_str(), "rb");
		if (file) {
			fseek(file, 0, SEEK_END);
			bytes = ftell(file);
			fclose(file);
		}

		else {
			ReportError(errorCode, filePath);
		}

		return bytes;
	}
}

	using namespace util;
}
