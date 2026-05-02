#pragma once
#include <string>
#include <vector>
#include "base.h"

int ldFileData(std::vector<skyScene>& skySceneS,
	std::vector<std::string>& use_file,
	std::vector<std::string>& no_keyword,
	std::string& use_language,
	std::fstream& LogFile);