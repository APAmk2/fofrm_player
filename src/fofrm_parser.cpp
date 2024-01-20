// fofrm_parser.cpp - Main parser code
// Copyright (C) 2024 APAMk2

#include "fofrm_parser.h"

#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

void DelimateStr(string input, string& key, string& val) {
	bool isKeyParsed = false;
	for (char c : input) {
		if (c == '=') {
			isKeyParsed = true;
			continue;
		}
		if (!isKeyParsed) {
			key += c;
		}
		else {
			val += c;
		}
	}
}

void LoadFofrm(filesystem::path file, unordered_map<string, string>& output) {
	string currString;
	vector<string> fofrmStrs;
	ifstream input(file.string());
	if (input.is_open()) {
		while (getline(input, currString)) {
			fofrmStrs.push_back(currString);
		}
	}
	input.close();

	for (size_t i = 0; i < fofrmStrs.size(); i++) {
		string key, val;
		DelimateStr(fofrmStrs[i], key, val);
		output[key] = val;
	}
}

