#pragma once

#include <filesystem>
#include <zaf/application.h>

std::filesystem::path GetTemplateDirectoryPath();
std::filesystem::path GetWorkingDirectoryPath();
std::filesystem::path GetInputDirectoryPath();

void RunZrcb(const std::wstring& arguments);

std::wstring FindDefaultOutputRCFile();

bool CheckOutputRC(
    const std::wstring& file_name,
    const std::vector<std::pair<std::wstring, std::wstring>>& expected_items);