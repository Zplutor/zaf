#pragma once

#include <filesystem>
#include <zaf/application.h>
#include <zaf/base/error/system_error.h>

std::filesystem::path GetTemplateDirectoryPath();
std::filesystem::path GetInputDirectoryPath();
std::filesystem::path GetOutputDirectoryPath();

void RunZrcb(const std::wstring& arguments);

std::wstring FindDefaultOutputRCFile();

bool CheckOutputRC(
    const std::wstring& file_name,
    const std::vector<std::pair<std::wstring, std::wstring>>& expected_items);