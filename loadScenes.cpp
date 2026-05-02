#include "loadScenes.h"
#include "agreements.h"
#include <fstream>
#include <map>

int loadDataDB(std::vector<std::string>& LightDataDB,
	std::vector<std::string>& TpDataDB,
	std::fstream& LogFile,
	std::vector<skyLight>& LightData,
	std::vector<skyTp>& TpData);
bool loadFileDateF(std::string datef_V,
	std::string Sdate_V,
	std::string Edate_V,
	std::fstream& LogFile);

int ldFileData(std::vector<skyScene>& skySceneS,
	std::vector<std::string>& use_file,
	std::vector<std::string>& no_keyword,
	std::string& use_language,
	std::fstream& LogFile) {
	auto AutoLoads = base::fio::file_read_lines("autoload.atoola1.config");
	for (auto& tmpAutoLoads : AutoLoads) {
		use_file.push_back(tmpAutoLoads);
	}
	int scene_code; skyName name; bool APBS; std::vector<skyLight>LightData; std::vector<skyTp>TpData;
	std::vector<std::string>LightDataDB;std::vector<std::string>TpDataDB;
	std::string loadFileDateF_1, loadFileDateF_2, loadFileDateF_3;
	for (auto& tmpUF : use_file) {
		scene_code = -1; name.cn = "未知"; name.en = "Unknown"; APBS = false; LightData = {}; TpData = {};
		LightDataDB = {}; TpDataDB = {};
		bool Is_Skip = false;
		loadFileDateF_1 = ""; loadFileDateF_2 = ""; loadFileDateF_3 = "";
		//Ensure data reset
		LogFile << SL::time::current_time_with_offset() << "\nLoad File: " << tmpUF << "\n";
		auto FileDataKV = base::fio::file_read_lines(tmpUF);
		for (auto& tmpFileDataKV : FileDataKV) {
			auto tmpFileDataKVKandV = base::data_process::part_str_once(tmpFileDataKV, "=");
			if (tmpFileDataKVKandV.empty()) {
				LogFile << SL::time::current_time_with_offset() << "\tMiss Datas, this file will be skipped!\n";
				LogFile << SL::time::current_time_with_offset() << 1001 << std::endl;//报错误代码时，直接报
				Is_Skip = true;
				continue;
			}
			if (tmpFileDataKVKandV[0] == "scene-code") {
				if (tmpFileDataKVKandV.size() < 2 || tmpFileDataKVKandV[1] == "") {
					LogFile << SL::time::current_time_with_offset() << "\tKey data scene-code is missing!\n";
					LogFile << SL::time::current_time_with_offset() << -2000 << std::endl;
					LogFile << SL::time::current_time_with_offset() << "\tThis file will be skipped!\n";
					LogFile << SL::time::current_time_with_offset() << 1001 << std::endl;
					Is_Skip = true;
					break;
				}
				try {
					scene_code = std::stoi(tmpFileDataKVKandV[1]);
				}
				catch (const std::exception& e) {
					LogFile << SL::time::current_time_with_offset()<< "Type conversion failed:" << e.what() << std::endl;
					LogFile << SL::time::current_time_with_offset() << -2001 << std::endl;
					LogFile << SL::time::current_time_with_offset() << "\tThis file will be skipped!\n";
					LogFile << SL::time::current_time_with_offset() << 1001 << std::endl;
					Is_Skip = true;
					break;
				}
			}
			if (tmpFileDataKVKandV[0] == "name-cn") {
				if (tmpFileDataKVKandV.size() < 2 || tmpFileDataKVKandV[1] == "") {
					LogFile << SL::time::current_time_with_offset() << 1000 << std::endl;
				}
				else {
					name.cn = tmpFileDataKVKandV[1];
				}
			}
			if (tmpFileDataKVKandV[0] == "name-en") {
				if (tmpFileDataKVKandV.size() < 2 || tmpFileDataKVKandV[1] == "") {
					LogFile << SL::time::current_time_with_offset() << 1000 << std::endl;
				}
				else {
					name.en = tmpFileDataKVKandV[1];
				}
			}
			if (tmpFileDataKVKandV[0] == "APBS") {
				if (tmpFileDataKVKandV.size() < 2 ||(
					tmpFileDataKVKandV[1] != "Y" &&
					tmpFileDataKVKandV[1] != "y" &&
					tmpFileDataKVKandV[1] != "T" &&
					tmpFileDataKVKandV[1] != "t")) {
					LogFile << SL::time::current_time_with_offset() << 1000 << std::endl;
				}
				else {
					APBS = true;
				}
			}
			if (tmpFileDataKVKandV[0] == "LightData") {
				if (tmpFileDataKVKandV.size() < 2 ) {
					LogFile << SL::time::current_time_with_offset() << 1002 << std::endl;
				}
				else {
					auto tmpFileDataKVKandV_LightData 
						= base::data_process::part_str(tmpFileDataKVKandV[1], ";");
					for (auto& tmpLightDataDBF : tmpFileDataKVKandV_LightData) {
						LightDataDB.push_back(tmpLightDataDBF);
					}
				}
			}
			if (tmpFileDataKVKandV[0] == "TpData") {
				if (tmpFileDataKVKandV.size() < 2) {
					LogFile << SL::time::current_time_with_offset() << 1002 << std::endl;
				}
				else {
					auto tmpFileDataKVKandV_TpData
						= base::data_process::part_str(tmpFileDataKVKandV[1], ";");
					for (auto& tmpTpDataDBF : tmpFileDataKVKandV_TpData) {
						TpDataDB.push_back(tmpTpDataDBF);
					}
				}
			}
			if (tmpFileDataKVKandV[0] == "datef") {
				if (tmpFileDataKVKandV.size() < 2) {
					LogFile << SL::time::current_time_with_offset() << "\tKey data datef is missing!\n";
					LogFile << SL::time::current_time_with_offset() << -2000 << std::endl;
					LogFile << SL::time::current_time_with_offset() << "\tThis file will be skipped!\n";
					LogFile << SL::time::current_time_with_offset() << 1001 << std::endl;
					Is_Skip = true;
					break;
				}
				loadFileDateF_1 = tmpFileDataKVKandV[1];

			}if (tmpFileDataKVKandV[0] == "Sdate") {
				if (tmpFileDataKVKandV.size() < 2) {
					LogFile << SL::time::current_time_with_offset() << "\tKey data Sdate is missing!\n";
					LogFile << SL::time::current_time_with_offset() << -2000 << std::endl;
					LogFile << SL::time::current_time_with_offset() << "\tThis file will be skipped!\n";
					LogFile << SL::time::current_time_with_offset() << 1001 << std::endl;
					Is_Skip = true;
					break;
				}
				loadFileDateF_2 = tmpFileDataKVKandV[1];

			}if (tmpFileDataKVKandV[0] == "Edate") {
				if (tmpFileDataKVKandV.size() < 2) {
					LogFile << SL::time::current_time_with_offset() << "\tKey data Edate is missing!\n";
					LogFile << SL::time::current_time_with_offset() << -2000 << std::endl;
					LogFile << SL::time::current_time_with_offset() << "\tThis file will be skipped!\n";
					LogFile << SL::time::current_time_with_offset() << 1001 << std::endl;
					Is_Skip = true;
					break;
				}
				loadFileDateF_3 = tmpFileDataKVKandV[1];

			}
		}
		auto datefpass = loadFileDateF(loadFileDateF_1, loadFileDateF_2, loadFileDateF_3, LogFile);
		auto DBS__LT = loadDataDB(LightDataDB, TpDataDB, LogFile, LightData, TpData);
		if (!datefpass) {
			Is_Skip = !datefpass;//pass = true
		}
		if (LightData.empty() && TpData.empty()) {
			Is_Skip = true;
		}
		if (scene_code <= 0) {
			Is_Skip = true;
		}
		if (Is_Skip) {
			LogFile << SL::time::current_time_with_offset() << "\tThis file will be skipped!\n";
			LogFile << SL::time::current_time_with_offset() << 1001 << std::endl;
			continue;
		}//数据入栈前直接跳过

		skyScene sky__scene{
			scene_code,
			name,
			APBS,
			LightData,
			TpData
		};
		skySceneS.push_back(sky__scene);
	}
	return 0;
}

bool loadFileDateF(std::string datef_V,
	std::string Sdate_V,
	std::string Edate_V,
	std::fstream& LogFile) {
	//TODO:待实现
}

int loadDataDB(std::vector<std::string>& LightDataDB,
	std::vector<std::string>& TpDataDB,
	std::fstream& LogFile,
	std::vector<skyLight>& LightData,
	std::vector<skyTp>& TpData) {
	//TODO:待实现
}
