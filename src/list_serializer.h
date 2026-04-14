#pragma once

#include <filesystem>

#include "list.h"

void SerializeToBin(const List* list, const std::filesystem::path& output_path);

List* DeserializeFromBin(const std::filesystem::path& input_path); // For tests

List* DeserializeFromText(const std::filesystem::path& input_path);