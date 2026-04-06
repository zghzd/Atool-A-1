#include <iostream>
#include <fstream>
#include "agreements.h"
#include "mainrun.h"

auto procstartdate = base::time::date();
auto procstarttime = SL::time::current_time_with_offset();

int main(int argc, char* argv[]) {
	std::fstream LogFile((procstartdate+".log"), std::ios::app);
	if (!LogFile.is_open()) {
		std::cerr << "Unable to open the file!" << std::endl;
		return -1;
	}
	if (argc < 2) {
		std::cout<<
			"usage: Atool-A-1 --total_candle-<float> [--use-<file>] [--owned_candle-<float>] [--max_candle-<float>] [--no-<keyword>] (-en|-cn)"
			<< std::endl;
		return -1;
	}
	LogFile << "-----Program Start-----\nThe program started at " << procstarttime << std::endl;
	LogFile << "Arguments:\n";
	for (int i = 0; i < argc; ++i) {
		LogFile << "argv[" << i << "] = " << argv[i] << '\n';
	}
	std::vector<std::string> arguments(argv + 1, argv + argc);
	LogFile << SL::time::current_time_with_offset() << " Successfully copied: argv[] -> arguments\n";

	float total_candle = 0.0f;
	float owned_candle = 0.0f;
	float max_candle = 0.0f;
	std::vector<std::string> use_file, no_keyword;
	std::string use_language = "en";

	LogFile << SL::time::current_time_with_offset() << " Success: Initialized default configuration\n";
	for (auto& argu : arguments) {
		if (argu.size() >= 2 && argu[0] == '-' && argu[1] == '-') {
			std::string a_p = argu.substr(2);
			auto a_p_p = base::data_process::part_str_once(a_p, "-");
			if ((a_p_p[1] == "") || (a_p_p[1] == " ")) {
				LogFile << SL::time::current_time_with_offset() << " Error: Unprocessable parameter ->" << argu;
				return -1;
			}
			if (a_p_p[0] == "total_candle") {
				try {
					total_candle = std::stof(a_p_p[1]);
				}
				catch (const std::invalid_argument& e) {
					LogFile << SL::time::current_time_with_offset() << " Conversion failed: Invalid number format -> " << a_p_p[1] << "\n";
					return -1;
				}
				catch (const std::out_of_range& e) {
					LogFile << SL::time::current_time_with_offset() << " Conversion failed: value exceeds float range -> " << a_p_p[1] << "\n";
					return -1;
				}
			}
			if (a_p_p[0] == "owned_candle") {
				try {
					owned_candle = std::stof(a_p_p[1]);
				}
				catch (const std::invalid_argument& e) {
					LogFile << SL::time::current_time_with_offset() << " Conversion failed: Invalid number format -> " << a_p_p[1] << "\n";
					return -1;
				}
				catch (const std::out_of_range& e) {
					LogFile << SL::time::current_time_with_offset() << " Conversion failed: value exceeds float range -> " << a_p_p[1] << "\n";
					return -1;
				}
			}
			if (a_p_p[0] == "max_candle") {
				try {
					max_candle = std::stof(a_p_p[1]);
				}
				catch (const std::invalid_argument& e) {
					LogFile << SL::time::current_time_with_offset() << " Conversion failed: Invalid number format -> " << a_p_p[1] << "\n";
					return -1;
				}
				catch (const std::out_of_range& e) {
					LogFile << SL::time::current_time_with_offset() << " Conversion failed: value exceeds float range -> " << a_p_p[1] << "\n";
					return -1;
				}
			}
			if (a_p_p[0] == "use") {
				use_file.push_back(a_p_p[1]);
			}
			if (a_p_p[0] == "no") {
				no_keyword.push_back(a_p_p[1]);
			}
		}
		if (argu == "-cn") {
			use_language = "cn";
		}
	}
	LogFile << SL::time::current_time_with_offset() << " Configurations: total_candle -> " << total_candle
		<< ", owned_candle -> " << owned_candle
		<< ", max_candle -> " << max_candle
		<< ", use_language -> " << use_language;
	LogFile << ", use -> {";
	for (auto& tmp : use_file) {
		LogFile << tmp << ";";
	}
	LogFile << "}, no -> {";
	for (auto& tmp : no_keyword) {
		LogFile << tmp << ";";
	}
	LogFile << "}.\n";
	float req_candle = total_candle - owned_candle;
	if (req_candle <= 0) {
		LogFile << SL::time::current_time_with_offset() << " Information: The total demand is less than or equal to 0, no calculation is required." << std::endl;
		if (use_language == "en") { std::cout << "The total demand is less than or equal to 0, no calculation is required.\n"; }
		if (use_language == "cn") { std::cout << "总需求量小于等于0，无需计算.\n"; }
		return 0;
	}
	if (req_candle > 100000) {
		LogFile << SL::time::current_time_with_offset() << " Error: Exceeded the maximum acceptable range." << std::endl;
		if (use_language == "en") { std::cout << "Exceeded the maximum acceptable range.\n"; }
		if (use_language == "cn") { std::cout << "超出最大可接受范围.\n"; }
		return -1;
	}
	int suc = mainrun(req_candle,
		max_candle,
		use_file,
		no_keyword,
		use_language,
		LogFile);
	if (suc != 0) {
		LogFile << SL::time::current_time_with_offset() << " Error: mainrun did not return normally." << std::endl;
		if (use_language == "en") { std::cout << "Error: mainrun did not return normally.\n"; }
		if (use_language == "cn") { std::cout << "错误：mainrun未正常返回.\n"; }
		return -1;
	}
	LogFile.close();
	return 0;
}
