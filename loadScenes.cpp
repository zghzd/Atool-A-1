#include "loadScenes.h"
#include "agreements.h"
#include <fstream>
#include <map>

bool hasSubstring(const std::vector<std::string>& strs, const std::string& str) {
	for (const auto& sub : strs) {
		if (str.find(sub) != std::string::npos) {
			return true;
		}
	}
	return false;
}

int loadDataDB(std::vector<std::string>& LightDataDB,
	std::vector<std::string>& TpDataDB,
	std::fstream& LogFile,
	std::vector<skyLight>& LightData,
	std::vector<skyTp>& TpData,
	std::vector<std::string>& no_keyword);
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
		auto DBS__LT = loadDataDB(LightDataDB, TpDataDB, LogFile, LightData, TpData, no_keyword);
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
			Is_Skip = false;
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
	std::vector<skyTp>& TpData,
	std::vector<std::string>& no_keyword) {
	std::vector<skyLight>preLightData; skyLight preLD;
	std::vector<skyTp> preTpData; skyTp preTD;
	//load LightDataDB
	for (auto& DBfilename : LightDataDB) {
		auto filedatas = base::fio::file_read_lines(DBfilename);
		if (filedatas.empty()) {
			LogFile << SL::time::current_time_with_offset() << -2000 << std::endl;
			LogFile << SL::time::current_time_with_offset() << "\tThis file will be skipped!\n";
			LogFile << SL::time::current_time_with_offset() << 1001 << std::endl;
			continue;
		}

		for (auto& linedata : filedatas) {
			auto datas = base::data_process::part_str(linedata, "\t");
			coordinate XYZ__LD {0.0f,0.0f,0.0f}; float NN__LN = 0.0; skyName NAME__L;
			if (datas.size() < 2 ) {
				LogFile << SL::time::current_time_with_offset() << -2000 << std::endl;
				LogFile << SL::time::current_time_with_offset() << "\tThis line will be skipped!\n";
				LogFile << SL::time::current_time_with_offset() << 1001 << std::endl;
				continue;
			}
			else if (datas.size() == 2) {
				try {
					NN__LN = std::stof(datas[1]);
					auto tmp__d__l = base::data_process::part_str(datas[0], ",");
					XYZ__LD = {
						std::stof(tmp__d__l[0]),std::stof(tmp__d__l[1]),std::stof(tmp__d__l[2])
					};
				}
				catch (const std::exception& e) {
					LogFile << SL::time::current_time_with_offset() << "Type conversion failed:" << e.what() << std::endl;
					LogFile << SL::time::current_time_with_offset() << -2001 << std::endl;
					LogFile << SL::time::current_time_with_offset() << "\tThis line will be skipped!\n";
					LogFile << SL::time::current_time_with_offset() << 1001 << std::endl;
					continue;
				}
				if (hasSubstring(no_keyword,"未知")|| hasSubstring(no_keyword, "Unknown") ){
					LogFile << SL::time::current_time_with_offset() << "User requested to skip.\n" << std::endl;
					LogFile << SL::time::current_time_with_offset() << "\tThis line will be skipped!\n";
					LogFile << SL::time::current_time_with_offset() << 1001 << std::endl;
					continue;
				}
				preLD = { XYZ__LD ,NN__LN ,NAME__L };
			}
			else if (datas.size() == 3) {
				try {
					NN__LN = std::stof(datas[1]);
					auto tmp__d__l = base::data_process::part_str(datas[0], ",");
					XYZ__LD = {
						std::stof(tmp__d__l[0]),std::stof(tmp__d__l[1]),std::stof(tmp__d__l[2])
					};
				}
				catch (const std::exception& e) {
					LogFile << SL::time::current_time_with_offset() << "Type conversion failed:" << e.what() << std::endl;
					LogFile << SL::time::current_time_with_offset() << -2001 << std::endl;
					LogFile << SL::time::current_time_with_offset() << "\tThis line will be skipped!\n";
					LogFile << SL::time::current_time_with_offset() << 1001 << std::endl;
					continue;
				}
				NAME__L.cn = datas[2];
				if (hasSubstring(no_keyword, NAME__L.cn) || hasSubstring(no_keyword, "Unknown")) {
					LogFile << SL::time::current_time_with_offset() << "User requested to skip.\n" << std::endl;
					LogFile << SL::time::current_time_with_offset() << "\tThis line will be skipped!\n";
					LogFile << SL::time::current_time_with_offset() << 1001 << std::endl;
					continue;
				}
				preLD = { XYZ__LD ,NN__LN ,NAME__L };
			}
			else if (datas.size() == 4) {
				try {
					NN__LN = std::stof(datas[1]);
					auto tmp__d__l = base::data_process::part_str(datas[0], ",");
					XYZ__LD = {
						std::stof(tmp__d__l[0]),std::stof(tmp__d__l[1]),std::stof(tmp__d__l[2])
					};
				}
				catch (const std::exception& e) {
					LogFile << SL::time::current_time_with_offset() << "Type conversion failed:" << e.what() << std::endl;
					LogFile << SL::time::current_time_with_offset() << -2001 << std::endl;
					LogFile << SL::time::current_time_with_offset() << "\tThis line will be skipped!\n";
					LogFile << SL::time::current_time_with_offset() << 1001 << std::endl;
					continue;
				}
				NAME__L.cn = datas[2]; NAME__L.en = datas[3];
				if (hasSubstring(no_keyword, NAME__L.cn) || hasSubstring(no_keyword, NAME__L.en)) {
					LogFile << SL::time::current_time_with_offset() << "User requested to skip.\n" << std::endl;
					LogFile << SL::time::current_time_with_offset() << "\tThis line will be skipped!\n";
					LogFile << SL::time::current_time_with_offset() << 1001 << std::endl;
					continue;
				}
				preLD = { XYZ__LD ,NN__LN ,NAME__L };
			}
			else {
				LogFile << SL::time::current_time_with_offset() << "\tToo much data to process!\n";
				LogFile << SL::time::current_time_with_offset() << "\tThis line will be skipped!\n";
				LogFile << SL::time::current_time_with_offset() << 1001 << std::endl;
				continue;
			}
			preLightData.push_back(preLD);
		}
	}
	
	//load TpDataDB
	for (auto& DBfilename : TpDataDB) {
		auto filedatas = base::fio::file_read_lines(DBfilename);
		if (filedatas.empty()) {
			LogFile << SL::time::current_time_with_offset() << -2000 << std::endl;
			LogFile << SL::time::current_time_with_offset() << "\tThis file will be skipped!\n";
			LogFile << SL::time::current_time_with_offset() << 1001 << std::endl;
			continue;
		}

		for (auto& linedata : filedatas) {
			auto datas = base::data_process::part_str(linedata, "\t");
			coordinate XYZ__Tp{ 0.0f,0.0f,0.0f }; int NextCode = 0; coordinate nextXYZ__Tp{ 0.0f,0.0f,0.0f }; skyName NAME__T;
			if (datas.size() < 3) {
				LogFile << SL::time::current_time_with_offset() << -2000 << std::endl;
				LogFile << SL::time::current_time_with_offset() << "\tThis line will be skipped!\n";
				LogFile << SL::time::current_time_with_offset() << 1001 << std::endl;
				continue;
			}
			else if (datas.size() == 3) {
				try {
					NextCode = std::stoi(datas[1]);
					auto tmp__d__l = base::data_process::part_str(datas[0], ",");
					XYZ__Tp = {
						std::stof(tmp__d__l[0]),std::stof(tmp__d__l[1]),std::stof(tmp__d__l[2])
					};
					auto tmp__d___l = base::data_process::part_str(datas[2], ",");
					nextXYZ__Tp = {
						std::stof(tmp__d___l[0]),std::stof(tmp__d___l[1]),std::stof(tmp__d___l[2])
					};
				}
				catch (const std::exception& e) {
					LogFile << SL::time::current_time_with_offset() << "Type conversion failed:" << e.what() << std::endl;
					LogFile << SL::time::current_time_with_offset() << -2001 << std::endl;
					LogFile << SL::time::current_time_with_offset() << "\tThis line will be skipped!\n";
					LogFile << SL::time::current_time_with_offset() << 1001 << std::endl;
					continue;
				}
				if (hasSubstring(no_keyword, "未知") || hasSubstring(no_keyword, "Unknown")) {
					LogFile << SL::time::current_time_with_offset() << "User requested to skip.\n" << std::endl;
					LogFile << SL::time::current_time_with_offset() << "\tThis line will be skipped!\n";
					LogFile << SL::time::current_time_with_offset() << 1001 << std::endl;
					continue;
				}
				preTD = { XYZ__Tp,NextCode,nextXYZ__Tp,NAME__T };
			}
			else if (datas.size() == 4) {
				try {
					NextCode = std::stoi(datas[1]);
					auto tmp__d__l = base::data_process::part_str(datas[0], ",");
					XYZ__Tp = {
						std::stof(tmp__d__l[0]),std::stof(tmp__d__l[1]),std::stof(tmp__d__l[2])
					};
					auto tmp__d___l = base::data_process::part_str(datas[2], ",");
					nextXYZ__Tp = {
						std::stof(tmp__d___l[0]),std::stof(tmp__d___l[1]),std::stof(tmp__d___l[2])
					};
				}
				catch (const std::exception& e) {
					LogFile << SL::time::current_time_with_offset() << "Type conversion failed:" << e.what() << std::endl;
					LogFile << SL::time::current_time_with_offset() << -2001 << std::endl;
					LogFile << SL::time::current_time_with_offset() << "\tThis line will be skipped!\n";
					LogFile << SL::time::current_time_with_offset() << 1001 << std::endl;
					continue;
				}
				NAME__T.cn = datas[3];
				if (hasSubstring(no_keyword, NAME__T.cn) || hasSubstring(no_keyword, NAME__T.en)) {
					LogFile << SL::time::current_time_with_offset() << "User requested to skip.\n" << std::endl;
					LogFile << SL::time::current_time_with_offset() << "\tThis line will be skipped!\n";
					LogFile << SL::time::current_time_with_offset() << 1001 << std::endl;
					continue;
				}
				preTD = { XYZ__Tp,NextCode,nextXYZ__Tp,NAME__T };
			}
			else if (datas.size() == 5) {
				try {
					NextCode = std::stoi(datas[1]);
					auto tmp__d__l = base::data_process::part_str(datas[0], ",");
					XYZ__Tp = {
						std::stof(tmp__d__l[0]),std::stof(tmp__d__l[1]),std::stof(tmp__d__l[2])
					};
					auto tmp__d___l = base::data_process::part_str(datas[2], ",");
					nextXYZ__Tp = {
						std::stof(tmp__d___l[0]),std::stof(tmp__d___l[1]),std::stof(tmp__d___l[2])
					};
				}
				catch (const std::exception& e) {
					LogFile << SL::time::current_time_with_offset() << "Type conversion failed:" << e.what() << std::endl;
					LogFile << SL::time::current_time_with_offset() << -2001 << std::endl;
					LogFile << SL::time::current_time_with_offset() << "\tThis line will be skipped!\n";
					LogFile << SL::time::current_time_with_offset() << 1001 << std::endl;
					continue;
				}
				NAME__T.cn = datas[3]; NAME__T.en = datas[4];
				if (hasSubstring(no_keyword, NAME__T.cn) || hasSubstring(no_keyword, NAME__T.en)) {
					LogFile << SL::time::current_time_with_offset() << "User requested to skip.\n" << std::endl;
					LogFile << SL::time::current_time_with_offset() << "\tThis line will be skipped!\n";
					LogFile << SL::time::current_time_with_offset() << 1001 << std::endl;
					continue;
				}
				preTD = { XYZ__Tp,NextCode,nextXYZ__Tp,NAME__T };
			}
			else {
				LogFile << SL::time::current_time_with_offset() << "\tToo much data to process!\n";
				LogFile << SL::time::current_time_with_offset() << "\tThis line will be skipped!\n";
				LogFile << SL::time::current_time_with_offset() << 1001 << std::endl;
				continue;
			}
			preTpData.push_back(preTD);
		}
	}
	LightData.insert(LightData.end(), preLightData.begin(), preLightData.end());
	TpData.insert(TpData.end(), preTpData.begin(), preTpData.end());
	return 0;
}
