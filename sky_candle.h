#pragma once
#include <string>
#include <vector>
namespace sky_::candle {
	//scene===================================================================================
	enum sceneenum {
		S_HOME = 1,
		S_AVIARY_VILLAGE = 2,
		S_SAND_DUNES,
		S_DAWN_TEMPLE,
		S_DAWN_OVERLOOK,
		S_CAVE_OF_PROPHECIES,
		S_TRIAL_OF_WATER,
		S_TRIAL_OF_EARTH,
		S_TRIAL_OF_AIR,
		S_TRIAL_OF_FIRE,
		S_PRAIRIE_GATEWAY,
		S_BUTTERFLY_FIELDS,
		S_VILLAGES,
		S_PRAIRIE_HEIGHTS,
		S_PRAIRIE_TEMPLE,
		S_CAVES,
		S_BIRD_NEST,
		S_SANCTUARY_ISLANDS,
		//雨林：
	};
	//lights===================================================================================
	inline int	L_CANDLE = 1001;
	inline int L_CLAM = 1002;
	inline int L_DARKNESS_TREE = 1003;
	inline int L_DARKNESS_PLANT = 1004;
	inline int L_UNDERWATER_LIGHTS = 1005;
	//===================================================================================
	struct Axyz {
		float x;//right/left
		float y;//up/down
		float z;//front/back
	};
	struct light {
		int type;
		int num;
		Axyz light_xyz;
	};
	struct tp_scene {
		Axyz tp_xyz;//where
		int s_code;//goal scene code
		Axyz to_tp_xyz;//where you at next scene
	};
	struct scene {
		std::string name;
		int s_code;
		std::vector<light>s_light;
		std::vector<tp_scene>tps;
		int t;//time of every "tps"
	};
}