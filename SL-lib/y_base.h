#pragma once
#ifndef Y_BASE_H
#define Y_BASE_H
#include <iostream>
#include <vector>
#include <string>

namespace base {
	namespace fio {
		int file_write_new(std::string fpath, std::string str);
		int file_write_c(std::string fpath, std::string str);
		std::string file_read_all(std::string fpath);
		std::string file_read_line(std::string fpath, int line_num);//负数表示倒数行数，0为第一行
		std::vector<std::string>file_read_lines(std::string fpath);
	}
	namespace data_process {
		std::vector<std::string>getAllFiles(std::string root_path);
		bool is_dir_has_file(std::string root_path);
		std::vector<std::string>part_str(std::string str, std::string part_by);
		std::vector<std::string>part_str_once(std::string str, std::string part_by);
		void addIfNotExists_invector(std::vector<std::string>& vec, const std::string& str);
	}
	namespace time {
		std::string utc_p0800();//utc+0800
		std::string date();//YYYY-MM-DD
	}
	namespace logger {
		std::string E(std::string msg, std::string logpath = "./");
		std::string W(std::string msg, std::string logpath = "./");
		std::string D(std::string msg, std::string logpath = "./");
		std::string I(std::string msg, std::string logpath = "./");
	}
	namespace fio {
		std::vector<std::string> filterFilesByExtension(
			const std::vector<std::string>& files,
			const std::string& fileextname);
		std::string getFileExtension(const std::string& file);
		std::string transPathToDot(const std::string path);
		std::string removePrefix(const std::string& str, const std::string& prefix);
	}
}
#endif