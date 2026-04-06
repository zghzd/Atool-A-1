#pragma once
#include <vector>
#include <string>

struct name_cn_en {
	std::string cn;
	std::string en;
};
struct coordinate {
	float x;
	float y;
	float z;
};
struct skyLight {
	coordinate lightCoordinate;
	float lightNum;
	name_cn_en lightName;
};
struct sceneFrom {
	int from_scene_num;
	coordinate from_scene_coordinate;
	coordinate this_coordinate;
};
struct skyScene {
	int scene_num;
	name_cn_en scene_name;
	std::vector<sceneFrom>scene_from;
	std::vector<skyLight>scene_light;
	/*std::string baseline_time;
	int ableNum;*///硬编码的数据是常驻数据，不应出现此类现象
};