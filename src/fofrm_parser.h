// fofrm_parser.h - .fofrm parser header
// Copyright (C) 2024 APAMk2

#pragma once

#include <string>
#include <filesystem>
#include <unordered_map>

void DelimateStr(std::string input, std::string& key, std::string& val);

void LoadFofrm(std::filesystem::path file, std::unordered_map<std::string, std::string>& output);