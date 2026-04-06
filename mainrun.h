#pragma once
#include <string> 
#include <vector>

int mainrun(float req_candle,
	float max_candle,
	std::vector<std::string> use_file,
	std::vector<std::string> no_keyword,
	std::string use_language,
	std::fstream& LogFile);