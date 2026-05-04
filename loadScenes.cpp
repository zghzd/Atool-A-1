#include "loadScenes.h"
#include "agreements.h"
#include <fstream>
#include <map>

#include <chrono>
#include <regex>
#include <sstream>
#include <iomanip>
#include <cmath>


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

#pragma warning(push)
#pragma warning(disable : 4996)

static std::chrono::system_clock::time_point parseDateTimeToUTC(const std::string& s, std::fstream& LogFile) {
	std::regex pattern(R"((\d{4})-(\d{2})-(\d{2})T(\d{2}):(\d{2}):(\d{2})([+-])(\d{2})(\d{2}))");
	std::smatch match;
	if (!std::regex_match(s, match, pattern)) {
		LogFile << SL::time::current_time_with_offset() << " Invalid datetime format: " << s << std::endl;
		return std::chrono::system_clock::time_point::min();
	}

	int year = std::stoi(match[1]);
	int month = std::stoi(match[2]);
	int day = std::stoi(match[3]);
	int hour = std::stoi(match[4]);
	int minute = std::stoi(match[5]);
	int second = std::stoi(match[6]);
	char sign = match[7].str()[0];
	int offsetHour = std::stoi(match[8]);
	int offsetMinute = std::stoi(match[9]);
	int offsetSeconds = (offsetHour * 3600 + offsetMinute * 60);
	if (sign == '-') offsetSeconds = -offsetSeconds;

	std::tm tm = {};
	tm.tm_year = year - 1900;
	tm.tm_mon = month - 1;
	tm.tm_mday = day;
	tm.tm_hour = hour;
	tm.tm_min = minute;
	tm.tm_sec = second;
	tm.tm_isdst = -1;

	std::time_t tt = std::mktime(&tm);
	std::time_t utc_tt = _mkgmtime(&tm);
	if (utc_tt == -1) {
		LogFile << SL::time::current_time_with_offset() << " Failed to convert datetime to UTC: " << s << std::endl;
		return std::chrono::system_clock::time_point::min();
	}

	utc_tt -= offsetSeconds;
	return std::chrono::system_clock::from_time_t(utc_tt);
}

static std::chrono::year_month_day getCurrentLocalDate() {
	auto now = std::chrono::system_clock::now();
	std::time_t tt = std::chrono::system_clock::to_time_t(now);
	std::tm local_tm;
	localtime_s(&local_tm, &tt);
	return std::chrono::year_month_day{
		std::chrono::year(local_tm.tm_year + 1900),
		std::chrono::month(local_tm.tm_mon + 1),
		std::chrono::day(local_tm.tm_mday)
	};
}

static std::chrono::year_month_day timePointToLocalDate(std::chrono::system_clock::time_point tp) {
	std::time_t tt = std::chrono::system_clock::to_time_t(tp);
	std::tm local_tm;
	localtime_s(&local_tm, &tt);
	return std::chrono::year_month_day{
		std::chrono::year(local_tm.tm_year + 1900),
		std::chrono::month(local_tm.tm_mon + 1),
		std::chrono::day(local_tm.tm_mday)
	};
}

static int64_t daysBetween(const std::chrono::year_month_day& date1, const std::chrono::year_month_day& date2) {
	using namespace std::chrono;
	auto sys1 = sys_days(date1);
	auto sys2 = sys_days(date2);
	return (sys2 - sys1).count();
}

bool loadFileDateF(std::string datef_V,
	std::string Sdate_V,
	std::string Edate_V,
	std::fstream& LogFile) {
	auto trim = [](std::string& s) {
		s.erase(0, s.find_first_not_of(" \t\n\r"));
		s.erase(s.find_last_not_of(" \t\n\r") + 1);
		};
	trim(Sdate_V);
	trim(Edate_V);
	trim(datef_V);

	const auto TP_MIN = std::chrono::system_clock::time_point::min();
	const auto TP_MAX = std::chrono::system_clock::time_point::max();
	auto startTP = TP_MIN;
	auto endTP = TP_MAX;

	if (!Sdate_V.empty()) {
		startTP = parseDateTimeToUTC(Sdate_V, LogFile);
		if (startTP == TP_MIN) {
			LogFile << SL::time::current_time_with_offset() << " Failed to parse Sdate, treat as no start limit.\n";
			startTP = TP_MIN;
		}
	}
	if (!Edate_V.empty()) {
		endTP = parseDateTimeToUTC(Edate_V, LogFile);
		if (endTP == TP_MIN) {
			LogFile << SL::time::current_time_with_offset() << " Failed to parse Edate, treat as no end limit.\n";
			endTP = TP_MAX;
		}
	}

	auto nowUTC = std::chrono::system_clock::now();
	if (nowUTC < startTP || nowUTC > endTP) {
		LogFile << SL::time::current_time_with_offset() << " Current time is out of [Sdate, Edate] range.\n";
		return false;
	}

	if (!datef_V.empty()) {
		std::regex datefPattern(R"((\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2}[+-]\d{2}\d{2});(\d+))");
		std::smatch m;
		if (!std::regex_match(datef_V, m, datefPattern)) {
			LogFile << SL::time::current_time_with_offset() << " Invalid datef format: " << datef_V << ", treat as false.\n";
			return false;
		}
		std::string baseDateTimeStr = m[1];
		int divisor = std::stoi(m[2]);
		if (divisor <= 0) {
			LogFile << SL::time::current_time_with_offset() << " datef divisor must be positive, got " << divisor << ", treat as false.\n";
			return false;
		}
		auto baseUTC = parseDateTimeToUTC(baseDateTimeStr, LogFile);
		if (baseUTC == TP_MIN) {
			LogFile << SL::time::current_time_with_offset() << " Failed to parse base datetime in datef.\n";
			return false;
		}
		auto baseLocalDate = timePointToLocalDate(baseUTC);
		auto currentLocalDate = getCurrentLocalDate();

		int64_t diffDays = daysBetween(baseLocalDate, currentLocalDate);
		if (diffDays % divisor != 0) {
			LogFile << SL::time::current_time_with_offset() << " datef condition not satisfied: diffDays=" << diffDays
				<< ", divisor=" << divisor << "\n";
			return false;
		}
	}
	LogFile << SL::time::current_time_with_offset() << " loadFileDateF: all conditions satisfied.\n";
	return true;
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
