#pragma once
#include <vector>
#include <string>

struct skyName {
	std::string cn = "未知";
	std::string en = "Unknown";
};
struct coordinate {
	float x;
	float y;
	float z;
};
struct skyLight {
	coordinate cd;
	float num;
	skyName name;
};
struct skyTp {
	coordinate cd;
	int next_scene_code;
	coordinate next_scene_coordinate;
	skyName name;
};
struct skyScene {
	int sceneCode;
	skyName sceneName;
	bool APBS = false;
	std::vector<skyLight> sceneLight;
	std::vector<skyTp>sceneTp;
};