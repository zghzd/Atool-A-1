#pragma once
#include <string>

namespace SL {
	namespace time {
		std::string current_time_with_offset();
		int days_diff(const std::string& base_time, const std::string& other_time);
    }
}