#pragma once

#include <filesystem>
#include <optional>

#include "list.h"

void SerializeToBin(const ListNode& list, const std::filesystem::path& output_path = "outlet.out");

std::optional<ListNode> DeserializeFromBin(const std::filesystem::path& input_path = "outlet.out"); // For tests

std::optional<ListNode> DeserializeFromText(const std::filesystem::path& input_path = "inlet.in");